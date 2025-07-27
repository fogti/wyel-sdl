#pragma once
# include <SDL.h>
class WAverage final {
 public:
  typedef unsigned int value_t;
  typedef value_t weight_t;

 private:
  double _last;
  Uint32 _lticks;
  weight_t _weight;
  // sum: round 16 bytes

 public:
  WAverage() noexcept;

  void start(const value_t startval, const weight_t weight) noexcept;
  void push() noexcept;
  value_t get() const noexcept;
};
