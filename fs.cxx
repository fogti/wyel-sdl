#include <stdlib.h>
#include <SDL_filesystem.h>
#include <fs.hpp>

using namespace std;

string get_wyel_home() {
  char * const tmp = SDL_GetPrefPath("ZITE", "wyel-sdl");
  const string ret = (tmp ? tmp : ".");
  SDL_free(tmp);
  return ret;
}

string get_wyel_static_home() {
  char * const tmp = SDL_GetBasePath();
  const string ret = (tmp ? tmp : string());
  SDL_free(tmp);
  return ret;
}
