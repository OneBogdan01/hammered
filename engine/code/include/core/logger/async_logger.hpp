#pragma once
#include "base_logger.hpp"
#include "log_thread_pool.hpp"
#include "mutex.hpp"
namespace hm::log
{
enum class AsyncOverflowPolicy : u8
{
  BLOCK,
  OVERRUN_OLDEST,
  DISCARD_NEW
};
class AsyncLogger : hm::log::BaseLogger<hm::log::NullMutex>
{
 public:
  using BaseLogger<hm::log::NullMutex>::Log;
  void Log(hm::log::Level level, std::string_view msg) override {};
  void Flush() override {};

 private:
  hm::log::LogThreadPool m_threadPool;
};
} // namespace hm::log
