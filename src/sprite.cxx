#include "borders.hpp"
#include "direction.hpp"
#include "rect.hpp"
#include "sprite.hpp"

sprite::sprite(int x, int y, SDL_Texture *new_image, direction_t _d): collision(false), d(_d) {
  set_image(new_image);
  rect.x = x;
  rect.y = y;
}

void sprite::set_image(SDL_Texture *const new_image) noexcept {
  image = new_image;
  SDL_QueryTexture(image, 0, 0, &rect.w, &rect.h);
}

void sprite::move(int mx, int my) noexcept {
  rect.x += mx;
  rect.y += my;
}

void sprite::move(const direction_t &_d) noexcept {
  d = _d;
  direction2xyd(_d, rect.x, rect.y);
}

const SDL_Texture *sprite::get_image() const noexcept {
  return image;
}

bool sprite::draw(SDL_Renderer *const target) const noexcept {
  return SDL_RenderCopy(target, image, 0, &rect) == 0;
}

bool sprite::valid() const noexcept {
  return image && rect.x > 0 && rect.y > 0 && (rect.x + rect.w) < WYEL_MAX_X && (rect.y + rect.h) < WYEL_MAX_Y;
}

bool sprite::destroyed() const noexcept {
  return collision;
}

void sprite::notify_destroyed() noexcept {
  collision = true;
}

bool operator==(const sprite& a, const sprite& b) noexcept {
  return (a.rect == b.rect) && (a.get_image() == b.get_image());
}

bool operator!=(const sprite& a, const sprite& b) noexcept {
  return !(a == b);
}

bool is_hit(const sprite &a, const sprite &b) noexcept {
  return (a == b) || is_hit(a.rect, b.rect);
}
