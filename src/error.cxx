#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include <atomic>
#include "error.hpp"

extern std::atomic<bool> breakout;
using std::string;

void errmsg(const string &msg) {
  fprintf(stderr, "wyel-sdl: %s\n", msg.c_str());
  breakout = true;
  exit(1);
}

void sdl_xxx_errmsg(const char *const subsystem, const char *const fn) {
  errmsg(string(subsystem) + ": " + string(fn) + "() failed: " + SDL_GetError());
}
