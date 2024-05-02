#include <sys/prctl.h>
#include <stdlib.h>
#include <math.h>
#include <SDL_ttf.h>
#include <SDL_keyboard.h>
#include <SDL_scancode.h>
#include <ThreadPool.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "average.hpp"
#include "borders.hpp"
#include "config.hpp"
#include "error.hpp"
#include "font.hpp"
#include "img.hpp"
#include "lock.hpp"
#include "menu.hpp"
#include "rand.hpp"
#include "shot.hpp"

using namespace std;

wyel_config my_config;

// counters
atomic<bool> breakout, pause_mode;
static WAverage game_rps, game_fps;
static unsigned int lifed_rounds, killed_ships;
static atomic<unsigned> move_sleep, draw_sleep;

// ships
static ship *usership;
static vector<ship> ships;

// SDL base
SDL_Window *my_window;
SDL_Renderer *my_renderer;

// mutexes
// lock order: draw > (objch > usership)
mutex mutex_draw;
static mutex mutex_objch, mutex_usership;

static ThreadPool threadpool(std::max(thread::hardware_concurrency(), static_cast<unsigned>(1u)));

// SDL text
static SDL_Color state_text_color;
static SDL_Rect text_rect;
TTF_Font *text_font;

// SDL window size
int WYEL_MAX_X, WYEL_MAX_Y;

static unsigned int adjps_between(unsigned int sleep_time, const WAverage &aver, const WAverage::value_t minmax[2]) {
  const unsigned int ps = aver.get();
  if(ps > minmax[1]) ++sleep_time;
  else if(ps < minmax[0] && sleep_time > 0) --sleep_time;
  return sleep_time;
}

static void my_pause(const unsigned int sleep_time) {
  do {
    SDL_Delay(sleep_time);
  } while(pause_mode);
}

template<typename Ret>
Ret with_usership(const std::function<Ret (ship&)> &fn) {
  if(usership) {
    LOCK(usership);
    return fn(*usership);
  }
  return static_cast<Ret>(false);
}

template<>
void with_usership(const std::function<void (ship&)> &fn) {
  if(usership) {
    LOCK(usership);
    fn(*usership);
  }
}

static void mover() {
  move_sleep = usership ? 5 : 1;
  vector<future<void>> futvec;
  futvec.reserve(2 + ships.size());
  prctl(PR_SET_NAME, "mover", 0, 0, 0);
  while(!breakout) {
    game_rps.push();

    {
      LOCK(objch);
      futvec.emplace_back(threadpool.enqueue(move_shots));

      if(ships.size() < my_config.max_ships && rand() % my_config.spawn_probab == 0) ships.emplace_back();

      for(auto &i : ships) {
        futvec.emplace_back(threadpool.enqueue([&i]() {
          if(!i.valid()) return;
          if(i.destroyed() || is_ship_hit(i)) {
            i.notify_destroyed();
            return;
          }
          direction_t d = zsrand::get_direction();
          i.move(d);
          if(is_ship_hit(i)) {
            const auto agd = against_direction(d);
            i.move(agd);
            i.move(agd);
            i.move(d);
          }
        }));
      }

      for(auto &i : futvec)
        i.wait();
      futvec.clear();
      reserve_shots(2 + ships.size());
      for(auto &i : ships)
        i.fire();

      with_usership<void>([](ship &my_usership) {
        if(is_ship_hit(my_usership)) return;
        const auto mud = my_usership.d;
        const auto agmud = against_direction(mud);
        my_usership.move(mud);
        my_usership.fire();
        size_t hold = 10;
        do {
          my_usership.move(agmud);
          my_usership.move(agmud);
          my_usership.move(mud);
        } while(--hold && my_usership.valid() && is_ship_hit(my_usership));
      });
    }

    ++lifed_rounds;
    move_sleep = adjps_between(move_sleep, game_rps, my_config.rps);
    my_pause(move_sleep);
  }
}

static void redrawer() {
  draw_sleep = 15;
  prctl(PR_SET_NAME, "redrawer", 0, 0, 0);

  while(!breakout && !ships.empty()) {
    {
      // prevent deadlock
      LOCK(objch);
      if(with_usership<bool>([](ship &my_usership)
        { return is_ship_hit(my_usership); }
      )) break;
    }
    game_fps.push();

    {
      LOCK(draw);

      // background
      SDL_SetRenderDrawColor(my_renderer, 0, 0, 0, 255);
      SDL_RenderClear(my_renderer);

      // ships and shots
      {
        LOCK(objch);

        for(auto it = ships.begin(); it != ships.end();) {
          bool destruct = false;
          if(!it->valid()) destruct = true;
          else {
            it->draw(my_renderer);
            destruct = it->destroyed();
          }
          if(destruct) {
            ++killed_ships;
            it = ships.erase(it);
          } else {
            ++it;
          }
        }

        draw_shots(my_renderer);
      }

      // usership
      with_usership<void>([](ship &my_usership) {
        my_usership.draw(my_renderer);
      });

      // state text
      {
        SDL_Surface *state_text = TTF_RenderText_Solid(text_font, ("KS: " + to_string(killed_ships) + " | RPS: " + to_string(game_rps.get()) + " | FPS: " + to_string(game_fps.get())).c_str(), state_text_color);
        if(!state_text) ttf_errmsg("TTF_RenderText_Solid");

        SDL_Texture *texture = SDL_CreateTextureFromSurface(my_renderer, state_text);
        SDL_QueryTexture(texture, 0, 0, &text_rect.w, &text_rect.h);

        SDL_SetRenderDrawColor(my_renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(my_renderer, &text_rect);

        SDL_RenderCopy(my_renderer, texture, 0, &text_rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(state_text);
      }

      SDL_RenderPresent(my_renderer);
    }

    draw_sleep = adjps_between(draw_sleep, game_fps, my_config.fps);
    my_pause(draw_sleep);
  }
  breakout = true;
}

static void quit() {
  if(usership && usership != reinterpret_cast<ship*>(&usership))
    delete usership;

  usership = 0;

  img_db::cleanup();
  if(TTF_WasInit()) {
    TTF_CloseFont(text_font);
    TTF_Quit();
  }
  SDL_Quit();
  set_wyel_config(my_config);
  cout << "Rounds: " << lifed_rounds << '\n'
       << "Killed ships: " << killed_ships << '\n';
}

static void parse_arguments(const int argc, const char *const argv[]) {
  enum { APM_STD, APM_MSC } mode = APM_STD;

  bool have_usership = true;
  for(int i = 1; i < argc; ++i) {
    const string curarg = argv[i];
    bool valid = true;
    try {
      switch(mode) {
        case APM_MSC:
          {
            const unsigned x = stoi(curarg);
            if(x > 0) {
              my_config.max_ships = x;
              mode = APM_STD;
            } else valid = false;
          }
          break;

        default:
          if(curarg == "--help") {
            cout << "USAGE: wyel-sdl [--help] [no-usership] [--max-ships COUNT]\n"
                    "\n"
                    "Arguments:\n"
                    "  --help             this help text\n"
                    "  no-usership        passive / viewers mode (no user interaction, no user ship)\n"
                    "  --max-ships COUNT  set the maximum number of AI ships\n"
                    "\n";
            describe_wyel_config(cout);
            cout << "wyel-sdl by Alain Zscheile <fogti+devel@ytrizja.de>\n";
            exit(0);
          }
          else if(curarg == "no-usership") have_usership = false;
          else if(curarg == "--max-ships") mode = APM_MSC;
          else valid = false;
          break;
      }
    } catch(...) {
      valid = false;
    }
    if(!valid) {
      string my_error_msg = "invalid argument";

      if(mode == APM_MSC) my_error_msg += " (to --max-ships)";

      my_error_msg += ": " + curarg;
      errmsg(my_error_msg);
    }
  }

  // XXX hack: we delay creation of usership
  if(have_usership) usership = reinterpret_cast<ship*>(&usership);
}

int main(int argc, char *argv[]) {
  SDL_Event event;

  // init global variables
  breakout = pause_mode = false;
  lifed_rounds = killed_ships = 0;
  usership = 0;
  state_text_color = { 255, 0, 0, 255 };
  menu_text_color = { 255, 255, 255, 255 };
  text_rect = { 5, 5, 0, 0 };

  my_config = get_wyel_config();
  WYEL_MAX_X = my_config.winsize[0];
  WYEL_MAX_Y = my_config.winsize[1];

  parse_arguments(argc, argv);

  if(SDL_Init(SDL_INIT_VIDEO) == -1) sdl_errmsg("SDL_Init");

  zsrand::init();
  atexit(quit);

  game_fps.start(1, 100);
  game_rps.start(100, 100);

  my_window = SDL_CreateWindow("WYEL",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               WYEL_MAX_X, WYEL_MAX_Y,
                               SDL_WINDOW_RESIZABLE);
  if(!my_window) sdl_errmsg("SDL_CreateWindow");

  my_renderer = SDL_CreateRenderer(my_window, -1, 0);
  if(!my_renderer) sdl_errmsg("SDL_CreateRenderer");

  if(!img_db::init(my_renderer, my_config.datalocs)) errmsg("img_db_init() failed: images not found");
  SDL_SetRenderDrawBlendMode(my_renderer, SDL_BLENDMODE_NONE);

  if(TTF_Init() == -1) ttf_errmsg("TTF_Init");

  text_font = get_font(12);
  if(!text_font) ttf_errmsg("get_font via TTF_OpenFont");

  const bool have_usership = usership;
  if(usership) usership = new ship();

  ships.emplace_back();

  thread t_redraw(redrawer);
  thread t_move(mover);

  const auto key_sc_q = SDL_GetScancodeFromKey(SDLK_q);
  const auto key_sc_m = SDL_GetScancodeFromKey(SDLK_m);

  while(!breakout) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          breakout = true;
          break;

        case SDL_WINDOWEVENT:
          switch(event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
              {
                LOCK(draw);
                WYEL_MAX_X = event.window.data1;
                WYEL_MAX_Y = event.window.data2;
              }
              break;
          }
          break;
      }
    }

    const Uint8 *const keys = SDL_GetKeyboardState(0);
    if(breakout || keys[SDL_SCANCODE_ESCAPE] || keys[key_sc_q])
      break;
    if(keys[key_sc_m])
      menuer();

    if(have_usership) {
      with_usership<void>([keys](ship &my_usership) {
        if(keys[SDL_SCANCODE_UP]    || keys[SDL_SCANCODE_W])
          my_usership.move(UP);
        if(keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A])
          my_usership.move(LE);
        if(keys[SDL_SCANCODE_DOWN]  || keys[SDL_SCANCODE_S])
          my_usership.move(DO);
        if(keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
          my_usership.move(RI);
      });
    }
    SDL_Delay(std::min(move_sleep, draw_sleep) / 2 + 20);
  }
  breakout = true;
  t_redraw.join();
  t_move.join();

  return 0;
}
