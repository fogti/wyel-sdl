#pragma once
#include <string>
void errmsg(const std::string &msg);
void sdl_xxx_errmsg(const char *const subsystem, const char *const fn);
#define sdl_errmsg(fn) sdl_xxx_errmsg("SDL", fn);
#define ttf_errmsg(fn) sdl_xxx_errmsg("TTF", fn);
