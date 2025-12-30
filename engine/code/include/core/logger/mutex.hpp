#pragma once
namespace hm::log
{
struct NullMutex
{
  void lock() {}
  void unlock() {}
  bool try_lock() { return true; }
};
} // namespace hm::log
