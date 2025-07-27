#pragma once
#include "sprite.hpp"
struct ship : public sprite {
  ship();
  void move(const direction_t&) noexcept;
  void fire();
  void notify_destroyed() noexcept;
};
