#pragma once
#include "mutex.hpp"
#include "sinks.hpp"

namespace hm::log
{

template<typename Mutex>
struct BaseLogger
{
  virtual ~BaseLogger() = default;
  virtual void Log(Level level, std::string_view msg)
  {
    if (level < m_level)
      return;
    auto tid = std::this_thread::get_id();
    auto now = clock::now();
    LogMessage logMessage {.level = level,
                           .loggerName = m_name,
                           .payLoad = std::string(msg),
                           .timestamp = now,
                           .threadId = tid};
    std::scoped_lock lock(m_mutex);
    for (auto& sink : sinks)
    {
      sink->Sink(logMessage);
    }
  };

  virtual void Flush()
  {
    std::scoped_lock lock(m_mutex);
    for (auto& sink : sinks)
    {
      sink->Flush();
    }
  }

  template<typename... T>
  void Info(std::format_string<T...> fs, T&&... args)
  {
    HM_ZONE_SCOPED_N("  hm::log::Info");
    Log(Level::Info, std::format(fs, std::forward<T>(args)...));
  }

  template<typename... T>
  void Error(std::format_string<T...> fs, T&&... args)
  {
    HM_ZONE_SCOPED_N("Log::Error");
    Log(Level::Error, std::format(fs, std::forward<T>(args)...));
  }

  template<typename... T>
  void Warning(std::format_string<T...> fs, T&&... args)
  {
    HM_ZONE_SCOPED_N("Log::Warning");
    Log(Level::Warning, std::format(fs, std::forward<T>(args)...));
  }

  template<typename... T>
  void Debug(std::format_string<T...> fs, T&&... args)
  {
    HM_ZONE_SCOPED_N("Log::Debug");
    Log(Level::Debug, std::format(fs, std::forward<T>(args)...));
  }
  template<typename... T>
  void Debug(time_point ts, std::format_string<T...> fs, T&&... args)
  {
    HM_ZONE_SCOPED_N("Log::Debug");
    Log(Level::Debug, std::format(fs, std::forward<T>(args)...), ts);
  }
  std::vector<std::shared_ptr<BaseSink>> sinks;
  std::vector<LogMessage> buffer;

 protected:
  std::string m_name {"Global"};
  Level m_level {Level::Debug};
  Mutex m_mutex;
};
using LoggerMt = BaseLogger<std::mutex>;
using LoggerSt = BaseLogger<NullMutex>;
} // namespace hm::log
