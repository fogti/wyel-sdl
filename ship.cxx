#include <img.hpp>
#include <rand.hpp>
#include <ship.hpp>
#include <shot.hpp>

ship::ship(): sprite(0, 0, wyel_images.i_ship, DO) {
  do {
    set_pos(zsrand::get_x(), zsrand::get_y());
  } while(!valid());
}

void ship::move(const direction_t &_d) noexcept {
  const int _x = rect.x;
  const int _y = rect.y;

  sprite::move(_d);

  if(!valid()) {
    rect.x = _x;
    rect.y = _y;
  }
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
