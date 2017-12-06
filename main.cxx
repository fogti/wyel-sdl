#include <stdlib.h>
#include <math.h>
#include <SDL_thread.h>
#include <SDL_ttf.h>
#include <SDL_keyboard.h>
#include <SDL_scancode.h>

#include <atomic>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <utility>

#include <average.hpp>
#include <borders.hpp>
#include <config.hpp>
#include <error.hpp>
#include <font.hpp>
#include <img.hpp>
#include <lock.hpp>
#include <menu.hpp>
#include <perlin_noise.hpp>
#include <rand.hpp>
#include <shot.hpp>

using namespace std;

wyel_config my_config;

// counters
atomic<bool> breakout, pause_mode;
static WAverage game_rps, game_fps;
static unsigned int lifed_rounds, killed_ships;

// noise field
typedef map<unsigned int, map<unsigned int, unsigned char>> noise_field_t;
static noise_field_t prod_noise_field;

// ships
static ship *usership;
static vector<ship> ships;

// SDL base
SDL_Window *my_window;
SDL_Renderer *my_renderer;

// mutexes
// lock order: draw > (objch > usership)
Mutex mutex_draw;
static Mutex mutex_objch, mutex_usership;

// SDL text
static SDL_Color state_text_color;
static SDL_Rect text_rect;
TTF_Font *text_font;

// SDL window size
int WYEL_MAX_X, WYEL_MAX_Y;

static void adjps_between(unsigned int &sleep_time, const WAverage &aver, const WAverage::value_t minmax[2]) {
  const unsigned int ps = aver.get();
  if(ps > minmax[1]) ++sleep_time;
  else if(ps < minmax[0] && sleep_time > 1) --sleep_time;
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

static int mover(void *dummy) {
  unsigned int move_sleep = usership ? 5 : 1;
  bool move_ships = false;
  while(!breakout) {
    move_ships = !move_ships;
    game_rps.push();

    {
      LOCK(objch);
      move_shots();

      for(auto &i : ships)
        if(is_ship_hit(i)) i.notify_destroyed();

      if(rand() % my_config.spawn_probab == 0 && ships.size() < my_config.max_ships) ships.emplace_back();

      if(move_ships) {
        for(auto &i : ships) {
          if(!i.valid() || i.destroyed()) continue;
          direction_t d = zsrand::get_direction();
          i.move(d);
          if(is_ship_hit(i)) {
            i.move(against_direction(d));
            i.move(against_direction(d));
            i.move(d);
          }
          i.fire();
        }

        with_usership<void>([](ship &my_usership) {
          my_usership.fire();
        });
      }

      cleanup_shots();
      ++lifed_rounds;
    }

    adjps_between(move_sleep, game_rps, my_config.rps);
    my_pause(move_sleep);
  }
  return 0;
}

static void noise_pause() {
  do {
    SDL_Delay(my_config.noise_speed / 100 + 1);
  } while(!breakout && (pause_mode || !my_config.noise_prec));
}

static int noiser(void *dummy) {
  PerlinNoise pn(rand());

  // pause here to prevent div by zero error
  noise_pause();

  while(!breakout) {
    const double cur_ticks = static_cast<double>(SDL_GetTicks()) / my_config.noise_speed;
    const double max_x = static_cast<double>(WYEL_MAX_X) / my_config.noise_prec;
    const double max_y = static_cast<double>(WYEL_MAX_Y) / my_config.noise_prec;
    noise_field_t tmp_noise_field;

    for(int x = 0; x < max_x; ++x)
      for(int y = 0; y < max_y; ++y) {
        const double cur_noise = pn.noise(static_cast<double>(x) / max_x, static_cast<double>(y) / max_y, cur_ticks) * cur_ticks;
        tmp_noise_field[x * my_config.noise_prec][y * my_config.noise_prec] = 255 * (cur_noise - floor(cur_noise));
      }

    {
      LOCK(draw);
      prod_noise_field = move(tmp_noise_field);
    }
    noise_pause();
  }
  return 0;
}

static int redrawer(void *dummy) {
  unsigned int draw_sleep = 15;

  while(!breakout && !ships.empty()) {
    {
      // prevent deadlock
      LOCK(objch);
      if(with_usership<bool>([](ship &my_usership) {
        return is_ship_hit(my_usership);
      })) break;
    }
    game_fps.push();

    {
      LOCK(draw);

      // background
      SDL_SetRenderDrawColor(my_renderer, 0, 0, 0, 255);
      SDL_RenderClear(my_renderer);

      // noise
      if(my_config.noise_prec) {
        for(auto &&x : prod_noise_field)
          for(auto &&y : x.second) {
            SDL_SetRenderDrawColor(my_renderer, 0, y.second, y.second, 255);
            SDL_RenderDrawPoint(my_renderer, x.first, y.first);
          }
      }

      // ships and shots
      {
        LOCK(objch);
        draw_shots(my_renderer);

        for(auto &&i : ships) {
          i.draw(my_renderer);
          if(i.destroyed()) ++killed_ships;
        }

        ships.erase(
          remove_if(ships.begin(), ships.end(), [](const ship &s) noexcept -> bool {
            return s.destroyed() || !s.valid();
          }),
          ships.end());
      }

      // usership
      with_usership<void>([](ship &my_usership) {
        my_usership.draw(my_renderer);
      });

      // state text
      {
        LOCK(objch);

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

    adjps_between(draw_sleep, game_fps, my_config.fps);
    my_pause(draw_sleep);
  }
  breakout = true;
  return 0;
}

static void quit() {
  delete usership; usership = 0;

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
  int i = 1;

  for(; i < argc; ++i) {
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
            cout << "wyel-sdl by Erik Zscheile <erik.zscheile.@gmail.com>\n";
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

  if(usership) usership = new ship();

  ships.emplace_back();

  SDL_Thread *t1 = SDL_CreateThread(redrawer, "redrawer", 0);
  if(!t1) sdl_errmsg("SDL_CreateThread");

  SDL_Thread *t2 = SDL_CreateThread(mover, "mover", 0);
  if(!t2) sdl_errmsg("SDL_CreateThread");

  SDL_Thread *t3 = SDL_CreateThread(noiser, "noiser", 0);
  if(!t3) sdl_errmsg("SDL_CreateThread");

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

    const Uint8 *keys = SDL_GetKeyboardState(0);
    if(keys[SDL_SCANCODE_ESCAPE] || keys[key_sc_q]) breakout = true;
    if(breakout) break;
    if(keys[key_sc_m]) menuer();

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
    SDL_Delay(30);
  }
  breakout = true;
  SDL_WaitThread(t1, 0);
  SDL_WaitThread(t2, 0);
  SDL_WaitThread(t3, 0);

  return 0;
}
