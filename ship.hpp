#ifndef SHIP_HPP
# define SHIP_HPP 1
# include "sprite.hpp"
struct ship : public sprite {
  ship();
  void move(const direction_t&) noexcept;
  void fire();
  void notify_destroyed() noexcept;
};
#endif
