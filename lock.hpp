#pragma once
#include <mutex>
#define LOCK(MUTEX) std::unique_lock<std::mutex> l__##MUTEX(mutex_##MUTEX)
