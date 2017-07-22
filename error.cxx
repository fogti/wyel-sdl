#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include <atomic>

#include <error.hpp>

extern std::atomic<bool> breakout;

void errmsg(const std::string &msg) {
  fprintf(stderr, "wyel-sdl: %s\n", msg.c_str());
  breakout = true;
  exit(1);
}

void sdl_errmsg(const char *const fn) {
  errmsg("SDL: " + std::string(fn) + "() failed: " + SDL_GetError());
}

void ttf_errmsg(const char *const fn) {
  errmsg("TTF: " + std::string(fn) + "() failed: " + TTF_GetError());
}
