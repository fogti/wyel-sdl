#ifndef LOCK_HPP
# define LOCK_HPP 1
# include <SDL_thread.h>
class Mutex;

class Lock {
  friend class Mutex;
  SDL_mutex *&_m;
  Lock(SDL_mutex *&m) noexcept;

 public:
  ~Lock() noexcept;
};

class Mutex {
 public:
  SDL_mutex *m;

  Mutex() noexcept;
  ~Mutex() noexcept;

  Lock get_lock() noexcept;
};
#endif
