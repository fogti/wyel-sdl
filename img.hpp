#pragma once
#include <SDL.h>
#include <vector>
#include <string>

struct wyel_img_db {
  SDL_Texture *i_ship, *i_ship_destroyed, *i_shot, *i_shot_destroyed;
};

extern wyel_img_db wyel_images;

namespace img_db {
  bool init(SDL_Renderer* const renderer, const std::vector<std::string> &locations);
  void cleanup() noexcept;
}
