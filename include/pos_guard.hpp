#ifndef POS_GUARD_HPP
# define POS_GUARD_HPP 1
class PosGuard {
  int &_rx;
  int &_ry;
  const int _sx;
  const int _sy;
  bool _enabled;

 public:
  PosGuard(int &x, int &y) noexcept;
  ~PosGuard() noexcept;
  void disable();
};
#endif
