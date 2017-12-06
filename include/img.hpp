#ifndef IMG_HPP
# define IMG_HPP 1
# include <SDL.h>
# include <vector>
# include <string>

struct wyel_img_db {
  SDL_Texture *i_ship, *i_ship_destroyed, *i_shot, *i_shot_destroyed;
};

extern wyel_img_db wyel_images;

namespace img_db {
  bool init(SDL_Renderer* renderer, const std::vector<std::string> &locations);
  void cleanup() noexcept;
}
#endif
