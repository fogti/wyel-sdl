#include <stdlib.h>
#include <SDL_filesystem.h>
#include <utility>
#include "fs.hpp"

using namespace std;

static string get_wyel_home_wrap(char * const tmp, const char * const dfl) {
  const string ret = (tmp ? tmp : dfl);
  SDL_free(tmp);
  return ret;
}

string get_wyel_home() {
  return get_wyel_home_wrap(SDL_GetPrefPath("ZITE", "wyel-sdl"), ".");
}

string get_wyel_static_home() {
  return get_wyel_home_wrap(SDL_GetBasePath(), "");
}
