#include <img.hpp>
#include <pos_guard.hpp>
#include <rand.hpp>
#include <ship.hpp>
#include <shot.hpp>

ship::ship(): sprite(0, 0, wyel_images.i_ship, DO) {
  do {
    rect.x = zsrand::get_x();
    rect.y = zsrand::get_y();
  } while(!valid());
}

void ship::move(const direction_t &_d) noexcept {
  PosGuard pg(rect.x, rect.y);
  sprite::move(_d);
  if(valid()) pg.disable();
}

void ship::fire() {
  int xd = 0, yd = 0;

  direction2xyd(d, xd, yd);
  fire_shot(rect.x + rect.w / 4 + xd * (rect.w / 2 + 4),
            rect.y + rect.h / 4 + yd * (rect.h / 2 + 4),
            d);
}

void ship::notify_destroyed() noexcept {
  sprite::notify_destroyed();
  set_image(wyel_images.i_ship_destroyed);
}
