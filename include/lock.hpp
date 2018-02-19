#ifndef LOCK_HPP
# define LOCK_HPP 1
# include <SDL_thread.h>
class Mutex;

class Lock final {
  friend class Mutex;
  SDL_mutex *_m;
  explicit Lock(SDL_mutex *m) noexcept;

 public:
  ~Lock() noexcept;
};

class Mutex final {
  SDL_mutex *_m;

 public:
  Mutex() noexcept;
  ~Mutex() noexcept;

  Lock get_lock() noexcept;
};

# define LOCK(MUTEX) Lock l__##MUTEX = mutex_##MUTEX.get_lock()
#endif
