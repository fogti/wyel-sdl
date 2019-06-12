#include "rect.hpp"

bool operator==(const SDL_Rect& a, const SDL_Rect& b) noexcept {
  return SDL_RectEquals(&a, &b);
}

bool operator!=(const SDL_Rect& a, const SDL_Rect& b) noexcept {
  return !SDL_RectEquals(&a, &b);
}

static bool is_axis_hit(const Sint16 &c, const Uint16 &as, const Uint16 &bs) noexcept {
  return ((0 < c) && ( c < as))
      || ((0 > c) && (-c < bs));
}

bool is_hit(const SDL_Rect &a, const SDL_Rect &b) noexcept {
  return is_axis_hit(b.x - a.x, a.w, b.w)
      && is_axis_hit(b.y - a.y, a.h, b.h);
}
