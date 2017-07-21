#include <pos_guard.hpp>

PosGuard::PosGuard(int &x, int &y) noexcept
  : _rx(x), _ry(y), _sx(x), _sy(y), _enabled(true) { }

PosGuard::~PosGuard() noexcept {
  if(_enabled) {
    _rx = _sx;
    _ry = _sy;
  }
}

void PosGuard::disable() {
  _enabled = false;
}
