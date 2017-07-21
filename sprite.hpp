#ifndef SPRITE_HPP
# define SPRITE_HPP 1
# include <SDL.h>
# include <direction.hpp>
class sprite {
 public:
  SDL_Rect rect;
  direction_t d;

  sprite(int x, int y, SDL_Texture *new_image, direction_t _d);
  virtual ~sprite() noexcept;
  void set_image(SDL_Texture *new_image) noexcept;
  void set_pos(int x, int y) noexcept;
  void move(int mx, int my) noexcept;
  void move(const direction_t &_d) noexcept;
  bool draw(SDL_Renderer *target) noexcept;
  bool valid() const noexcept;
  bool destroyed() const noexcept;

  virtual void notify_destroyed() noexcept;

  const SDL_Rect *get_rect() const noexcept;
  const SDL_Texture *get_image() const noexcept;

 protected:
  SDL_Texture *image;
  bool collision;
};

bool operator==(const sprite& a, const sprite& b) noexcept;
bool operator!=(const sprite& a, const sprite& b) noexcept;
bool is_hit(const sprite& a, const sprite& b) noexcept;
#endif
