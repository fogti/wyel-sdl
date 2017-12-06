#include <stdlib.h>
#include <iostream>
#include <img.hpp>

wyel_img_db wyel_images;

using namespace std;

static SDL_Texture *img_load_BMP(SDL_Renderer* renderer, const string &path, const string &filename) {
  SDL_Surface *const temp = SDL_LoadBMP((path + "/images/" + filename + ".bmp").c_str());

  if(temp) {
    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 0, 0));
    SDL_Texture *const image = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    return image;
  }
  return 0;
}

namespace img_db {

bool init(SDL_Renderer* renderer, const vector<string> &locations) {
  wyel_images.i_ship = 0;
  wyel_images.i_ship_destroyed = 0;
  wyel_images.i_shot = 0;
  wyel_images.i_shot_destroyed = 0;

  for(auto &&i : locations) {
    if(i.empty()) continue;
#define WYEL_DB_IMG_FILE(F) if(!wyel_images.i_##F) wyel_images.i_##F = img_load_BMP(renderer, i, #F);
    WYEL_DB_IMG_FILE(ship);
    WYEL_DB_IMG_FILE(ship_destroyed);
    WYEL_DB_IMG_FILE(shot);
    WYEL_DB_IMG_FILE(shot_destroyed);
  }

  return wyel_images.i_ship && wyel_images.i_ship_destroyed && wyel_images.i_shot && wyel_images.i_shot_destroyed;
}

void cleanup() noexcept {
  SDL_DestroyTexture(wyel_images.i_ship);
  SDL_DestroyTexture(wyel_images.i_shot);
  SDL_DestroyTexture(wyel_images.i_ship_destroyed);
  SDL_DestroyTexture(wyel_images.i_shot_destroyed);
}

}
