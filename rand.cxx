#include <stdlib.h>
#include <time.h>

#include <borders.hpp>
#include <rand.hpp>

namespace zsrand {

using ::rand;

void init() {
  ::srand(::time(0));
}

auto get_direction() noexcept -> direction_t {
  return static_cast<direction_t>(rand() % 4);
}

template<class T>
int get__xy(const T max) {
  return 1 + ((max - 10) * (rand() / (RAND_MAX + 1.0)));
}

auto get_x() noexcept -> int {
  return get__xy(WYEL_MAX_X);
}

auto get_y() noexcept -> int {
  return get__xy(WYEL_MAX_Y);
}

}
