#include <stdlib.h>
#include <time.h>

#include <borders.hpp>
#include <rand.hpp>

void zsrand::init() {
  srand(time(0));
}

auto zsrand::get_direction() noexcept -> direction_t {
  return static_cast<direction_t>(rand() % 4);
}

auto zsrand::get_x() noexcept -> int {
  return 1 + ((WYEL_MAX_X - 10) * (rand() / (RAND_MAX + 1.0)));
}

auto zsrand::get_y() noexcept -> int {
  return 1 + ((WYEL_MAX_Y - 10) * (rand() / (RAND_MAX + 1.0)));
}
