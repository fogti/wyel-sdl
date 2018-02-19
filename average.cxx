#include <cmath>
#include <SDL.h>
#include <average.hpp>

WAverage::WAverage() noexcept
  : _last(0), _lticks(0), _weight(1) { }

void WAverage::start(const value_t startval, const weight_t weight) noexcept {
  _last = startval;
  _lticks = SDL_GetTicks();
  _weight = weight;
}

void WAverage::push() noexcept {
  const auto nticks = SDL_GetTicks();
  _last  += (1000.f / (nticks - _lticks) - _last) / _weight;
  _lticks = nticks;
}

auto WAverage::get() const noexcept -> value_t {
  return std::round(_last);
}
