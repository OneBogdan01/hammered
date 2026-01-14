#pragma once
#include "base_logger.hpp"

#include <algorithm>
#include <mutex>
namespace hm::log

{
template<typename Mutex>
struct SortedLogger : BaseLogger<Mutex>
{
  using BaseLogger<Mutex>::Log;
  void Log(Level level, std::string_view msg, time_point ts)
  {
    if (level < this->m_level)
      return;
    LogMsgView view {.level = level,
                     .loggerName = this->m_name,
                     .payload = msg,
                     .timestamp = ts,
                     .threadId = std::this_thread::get_id()};
    std::scoped_lock lock(this->m_mutex);
    this->buffer.emplace_back(view);
  }
  void Flush() override
  {
    std::scoped_lock lock(this->m_mutex);
    std::sort(this->buffer.begin(), this->buffer.end());
    for (const auto& msg : buffer)
    {
      for (auto& sink : this->sinks)
      {
        sink->Sink(msg);
      }
    }
    buffer.clear();
    for (const auto& sink : this->sinks)
    {
      sink->Flush();
    }
  }

  template<typename... T>
  void Debug(time_point ts, std::format_string<T...> fs, T&&... args)
  {
    Log(Level::Debug, std::format(fs, std::forward<T>(args)...), ts);
  }
  template<typename... T>
  void Info(time_point ts, std::format_string<T...> fs, T&&... args)
  {
    Log(Level::Info, std::format(fs, std::forward<T>(args)...), ts);
  }
  template<typename... T>
  void Warning(time_point ts, std::format_string<T...> fs, T&&... args)
  {
    Log(Level::Warning, std::format(fs, std::forward<T>(args)...), ts);
  }
  template<typename... T>
  void Error(time_point ts, std::format_string<T...> fs, T&&... args)
  {
    Log(Level::Error, std::format(fs, std::forward<T>(args)...), ts);
  }
  std::vector<LogMessage> buffer;
};
using SortedLoggerMT = SortedLogger<std::mutex>;
using SortedLoggerST = SortedLogger<NullMutex>;
} // namespace hm::log
