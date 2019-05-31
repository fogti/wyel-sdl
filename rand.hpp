#ifndef RAND_HPP
# define RAND_HPP 1
# include "direction.hpp"
namespace zsrand {
  void init();
  direction_t get_direction() noexcept;
  int get_x() noexcept;
  int get_y() noexcept;
}
#endif
