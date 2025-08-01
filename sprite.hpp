#pragma once
#include <SDL.h>
#include <direction.hpp>
class sprite {
  SDL_Texture *image;
  bool collision;

 public:
  SDL_Rect rect;
  direction_t d;

  sprite(int x, int y, SDL_Texture *new_image, direction_t _d);
  virtual ~sprite() noexcept = default;
  void set_image(SDL_Texture *const new_image) noexcept;
  void move(int mx, int my) noexcept;
  void move(const direction_t &_d) noexcept;
  bool draw(SDL_Renderer *const target) const noexcept;
  bool valid() const noexcept;
  bool destroyed() const noexcept;

  virtual void notify_destroyed() noexcept;
  const SDL_Texture *get_image() const noexcept;
};

bool operator==(const sprite& a, const sprite& b) noexcept;
bool operator!=(const sprite& a, const sprite& b) noexcept;
bool is_hit(const sprite& a, const sprite& b) noexcept;
