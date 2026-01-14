#pragma once
#include "base_logger.hpp"
namespace hg::log
{
template<typename Mutex>
struct AsyncLogger : hm::log::BaseLogger<Mutex>
{
  using hm::log::BaseLogger<Mutex>::Log;
  void Log(hm::log::Level level, std::string_view msg) override {};
  void Flush() override {};
};
} // namespace hg::log
