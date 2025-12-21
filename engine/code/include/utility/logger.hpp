#pragma once
#include "core/thread_pool.hpp"

#include <print>
#include <format>
#include <mutex>

namespace hm::log
{
enum class Level
{
  Debug,
  Info,
  Warning,
  Error,
};
namespace color
{
constexpr const char* reset = "\033[0m";
constexpr const char* green = "\033[32m";
constexpr const char* red = "\033[31m";
constexpr const char* yellow = "\033[33m";
constexpr const char* purple = "\033[37m";
} // namespace color
using clock = std::chrono::steady_clock;
using time_point = clock::time_point;
struct LogMessage
{
  Level level;
  std::string loggerName;
  std::string payLoad;
  time_point timestamp;
  bool operator<(const LogMessage& other) const
  {
    return timestamp < other.timestamp;
  }
};
struct BaseSink
{
  virtual ~BaseSink() = default;
  virtual void Sink(const LogMessage& msg) = 0;
  virtual void Flush() = 0;
};
struct ConsoleSink : BaseSink
{
  void Sink(const LogMessage& msg) override;
  void Flush() override;
};
struct FileSink : BaseSink
{
  explicit FileSink(const std::string& fileName) : output_file(fileName) {}
  void Sink(const LogMessage& msg) override;
  void Flush() override;
  std::ofstream output_file;
};
struct Logger
{
  void Log(Level level, std::string_view msg);
  void Log(Level level, std::string_view msg, time_point ts);
  void Flush();
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

 private:
  std::string m_name {"Global"};
  Level m_level {Level::Debug};
  std::mutex m_mutex;
};
inline Logger& GetGlobalLogger()
{
  static Logger instance;
  return instance;
}

template<typename... T>
void Info(std::format_string<T...> fs, T&&... args)
{
  GetGlobalLogger().Info(fs, std::forward<T>(args)...);
}

template<typename... T>
void Error(std::format_string<T...> fs, T&&... args)
{
  GetGlobalLogger().Error(fs, std::forward<T>(args)...);
}

template<typename... T>
void Warning(std::format_string<T...> fs, T&&... args)
{
  GetGlobalLogger().Warning(fs, std::forward<T>(args)...);
}

template<typename... T>
void Debug(std::format_string<T...> fs, T&&... args)
{
  GetGlobalLogger().Debug(fs, std::forward<T>(args)...);
}
template<typename... T>
void Debug(time_point ts, std::format_string<T...> fs, T&&... args)
{
  GetGlobalLogger().Debug(ts, fs, std::forward<T>(args)...);
}

inline void Flush()
{
  GetGlobalLogger().Flush();
}

} // namespace hm::log
