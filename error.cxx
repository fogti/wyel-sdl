#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include <error.hpp>

void errmsg(const std::string &msg) {
  fprintf(stderr, "wyel-sdl: %s\n", msg.c_str());
  exit(1);
}

void sdl_errmsg(const std::string &fn) {
  errmsg("SDL: " + fn + "() failed: " + SDL_GetError());
}

void ttf_errmsg(const std::string &fn) {
  errmsg("TTF: " + fn + "() failed: " + TTF_GetError());
}
