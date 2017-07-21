#ifndef AVERAGE_HPP
# define AVERAGE_HPP 1
# include <SDL.h>
class WAverage {
 public:
  typedef unsigned int value_t;
  typedef unsigned int weight_t;

  WAverage() noexcept;

  void start(const value_t startval, const weight_t weight) noexcept;
  void push(const value_t val = 1) noexcept;
  auto get() const noexcept -> value_t;

 protected:
  double _last;
  Uint32 _lticks;
  weight_t _weight;
  // sum: round 16 bytes
};
#endif
