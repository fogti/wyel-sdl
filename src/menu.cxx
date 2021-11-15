#include <stdlib.h>
#include <SDL_ttf.h>
#include <SDL_keyboard.h>
#include <SDL_scancode.h>

#include <atomic>
#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "borders.hpp"
#include "config.hpp"
#include "font.hpp"
#include "lock.hpp"
#include "menu.hpp"

using namespace std;

extern wyel_config my_config;

// counters
extern atomic<bool> breakout, pause_mode;
extern mutex mutex_draw;

// SDL base
extern SDL_Window *my_window;
extern SDL_Renderer *my_renderer;

// SDL text
SDL_Color menu_text_color;
extern TTF_Font *text_font;

static SDL_Texture* get_menu_texture(const unsigned int text_space, int& text_height) {
  const vector<string> lines = {
    "FPS       from  " + to_string(my_config.fps[0]),
    "FPS       to    " + to_string(my_config.fps[1]),
    "RPS       from  " + to_string(my_config.rps[0]),
    "RPS       to    " + to_string(my_config.rps[1]),
    "max ships     = " + to_string(my_config.max_ships),
    "spawn probab  = " + to_string(my_config.spawn_probab),
    "window size x = " + to_string(my_config.winsize[0]),
    "window size y = " + to_string(my_config.winsize[1])
  };

  return CreateTextureFromText(my_renderer, text_font, lines, WYEL_MAX_X, WYEL_MAX_Y, menu_text_color, text_space, text_height);
}

static void draw_menu(const unsigned int text_space, int& text_height) {
  SDL_Rect menu_text_rect;
  menu_text_rect.x = 20;
  menu_text_rect.y = 10;

  SDL_SetRenderDrawColor(my_renderer, 0, 0, 0, 255);
  SDL_RenderClear(my_renderer);

  // generate menu text
  SDL_Texture *texture = get_menu_texture(text_space, text_height);

  SDL_QueryTexture(texture, 0, 0, &menu_text_rect.w, &menu_text_rect.h);
  SDL_RenderCopy(my_renderer, texture, 0, &menu_text_rect);
  SDL_DestroyTexture(texture);
  SDL_RenderPresent(my_renderer);
}

static void draw_menu_select(const unsigned int txtlh, const unsigned int n, bool selected) {
  SDL_Rect mdrp_rect;
  mdrp_rect.x = 5;
  mdrp_rect.y = 7 + txtlh * (n + 0.5);
  mdrp_rect.w = 5;
  mdrp_rect.h = 5;

  const Uint8 color = selected ? 255 : 0;

  SDL_SetRenderDrawColor(my_renderer, color, color, color, 255);
  SDL_RenderFillRect(my_renderer, &mdrp_rect);
}

static void draw_menu_edit(const string &thing) {
  SDL_Surface *surface = TTF_RenderText_Solid(text_font, ("-- " + thing + ": --").c_str(), menu_text_color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(my_renderer, surface);
  SDL_FreeSurface(surface);

  SDL_Rect menu_text_rect;
  menu_text_rect.x = 20;
  menu_text_rect.y = 10;
  SDL_QueryTexture(texture, 0, 0, &menu_text_rect.w, &menu_text_rect.h);

  SDL_SetRenderDrawColor(my_renderer, 0, 0, 0, 255);
  SDL_RenderClear(my_renderer);
  SDL_RenderCopy(my_renderer, texture, 0, &menu_text_rect);
  SDL_DestroyTexture(texture);
  SDL_RenderPresent(my_renderer);
}

static void draw_menu_input(const unsigned int txtlh, const string &input) {
  SDL_Surface *surface = TTF_RenderText_Solid(text_font, ("> " + input).c_str(), menu_text_color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(my_renderer, surface);
  SDL_FreeSurface(surface);

  SDL_Rect menu_text_rect, menu_clear_rect;
  menu_text_rect.x = 5;
  menu_text_rect.y = 10 + txtlh;
  SDL_QueryTexture(texture, 0, 0, &menu_text_rect.w, &menu_text_rect.h);
  menu_clear_rect = menu_text_rect;
  menu_clear_rect.w = WYEL_MAX_X - menu_clear_rect.x - 1;

  SDL_SetRenderDrawColor(my_renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(my_renderer, &menu_clear_rect);
  SDL_RenderCopy(my_renderer, texture, 0, &menu_text_rect);
  SDL_DestroyTexture(texture);
  SDL_RenderPresent(my_renderer);
}

static unsigned int* get_usi_edit_var(const unsigned int edit_num) {
  switch(edit_num) {
    case 0:  return &my_config.fps[0];
    case 1:  return &my_config.fps[1];
    case 2:  return &my_config.rps[0];
    case 3:  return &my_config.rps[1];
    case 4:  return &my_config.max_ships;
    case 5:  return &my_config.spawn_probab;
    default: return 0;
  }
}

static int* get_si_edit_var(const unsigned int edit_num) {
  switch(edit_num) {
    case 8:  return &my_config.winsize[0];
    case 9:  return &my_config.winsize[1];
    default: return 0;
  }
}

void menuer() {
  // menu constants
  static const vector<string> edit_screens = {
    "FPS from",
    "FPS   to",
    "RPS from",
    "RPS   to",
    "max ships",
    "spawn probabability",
    "window size x",
    "window size y",
  };

  static const map<SDL_Scancode, int> number_map = {
#define NUMKEY(N) { SDL_GetScancodeFromKey(SDLK_##N), N },
    NUMKEY(0)
    NUMKEY(1) NUMKEY(2) NUMKEY(3)
    NUMKEY(4) NUMKEY(5) NUMKEY(6)
    NUMKEY(7) NUMKEY(8) NUMKEY(9)
#undef NUMKEY
  };

  static const auto key_sc_bs = SDL_GetScancodeFromKey(SDLK_BACKSPACE);
  static const unsigned int text_space = 2;

  // main menu code

  // enter pause mode (pause other threads)
  pause_mode = true;

  // wait for other threads to pause
  SDL_Delay(200);

  unsigned int edit_num = 0;
  unsigned int number_input = 0;
  int text_height;
  SDL_Event event;
  bool edit = false;

  LOCK(draw);
  draw_menu(text_space, text_height);

  const unsigned int txtlh = text_space + text_height;
  draw_menu_select(txtlh, 0, true);
  SDL_RenderPresent(my_renderer);

  while(true) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          breakout = true;
          break;

        case SDL_WINDOWEVENT:
          switch(event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
              WYEL_MAX_X = event.window.data1;
              WYEL_MAX_Y = event.window.data2;
              if(edit) draw_menu_edit(edit_screens[edit_num]);
              else {
                draw_menu(text_space, text_height);
                draw_menu_select(txtlh, edit_num, true);
                SDL_RenderPresent(my_renderer);
              }
              break;
          }
          break;
      }
    }
    if(breakout) break;

    const Uint8 *keys = SDL_GetKeyboardState(0);

    if(edit) {
      if(keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_LEFT]) {
        // apply input
        unsigned int * usiptr = get_usi_edit_var(edit_num);
        int * siptr = get_si_edit_var(edit_num);
        if(usiptr)    *usiptr = number_input;
        else if(siptr) *siptr = number_input;
        edit = false;
        number_input = 0;
        draw_menu(text_space, text_height);
        draw_menu_select(txtlh, edit_num, true);
        SDL_RenderPresent(my_renderer);
      } else {
        int new_input = -1;

        for(auto &&i : number_map)
          if(keys[i.first])
            new_input = i.second;

        if(new_input != -1) {
          number_input *= 10;
          number_input += new_input;
          draw_menu_input(txtlh, to_string(number_input));
        } else if(keys[key_sc_bs]) {
          number_input /= 10;
          draw_menu_input(txtlh, to_string(number_input));
        }
      }
    } else {
      if(keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_LEFT]) break;
      const auto old_edit_num = edit_num;

      if(keys[SDL_SCANCODE_UP] && edit_num > 0)
        --edit_num;

      if(keys[SDL_SCANCODE_DOWN] && edit_num < (edit_screens.size() - 1))
        ++edit_num;

      if(old_edit_num != edit_num) {
        draw_menu_select(txtlh, old_edit_num, false);
        draw_menu_select(txtlh, edit_num,     true);
        SDL_RenderPresent(my_renderer);
      } else if(keys[SDL_SCANCODE_RIGHT]) {
        unsigned int * usiptr = get_usi_edit_var(edit_num);
        int * siptr = get_si_edit_var(edit_num);
        number_input = usiptr ? *usiptr : (siptr ? *siptr : 0);
        draw_menu_edit(edit_screens[edit_num]);
        draw_menu_input(txtlh, to_string(number_input));
        edit = true;
      }
    }

    SDL_Delay(75);
  }
  pause_mode = false;
  SDL_Delay(100);
}
