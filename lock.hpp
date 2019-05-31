#ifndef LOCK_HPP
# define LOCK_HPP 1
# include <mutex>
# define LOCK(MUTEX) std::unique_lock<std::mutex> l__##MUTEX(mutex_##MUTEX)
#endif
