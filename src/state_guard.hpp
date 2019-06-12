#pragma once
template<class T>
class StateGuard {
  T& _r;
  const T _s;
  bool _enabled;

 public:
  explicit StateGuard(T &o) noexcept
    : _r(o), _s(o), _enabled(true) { }

  virtual ~StateGuard() noexcept {
    if(_enabled) _r = _s;
  }

  void disable() noexcept {
    _enabled = false;
  }
};

template<class T>
auto make_StateGuard(T &o) noexcept {
  return StateGuard<T>(o);
}
