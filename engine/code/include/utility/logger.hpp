#pragma once
#include "external/tracy_impl.hpp"

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

struct LogMessage
{
  Level level;
  std::string_view loggerName;
  std::string_view payLoad;
};
struct BaseSink
{
  virtual ~BaseSink() = default;
  virtual void Sink(const LogMessage& msg) = 0;
};
struct ConsoleSink : BaseSink
{
  void Sink(const LogMessage& msg) override;
};
struct FileSink : BaseSink
{
  explicit FileSink(const std::string& fileName) : output_file(fileName) {}
  void Sink(const LogMessage& msg) override;
  std::ofstream output_file;
};
struct Logger
{
  void Log(Level level, std::string_view msg)
  {
    if (level < m_level)
      return;

    std::lock_guard lock(m_mutex);
    for (const auto& sink : m_sinks)
    {
      sink->Sink({level, m_name, msg});
    }
  }
  template<typename... T>
  void Info(std::format_string<T...> fs, T&&... args)
  {
    HM_ZONE_SCOPED_N("Log::Info");
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
  std::vector<std::shared_ptr<BaseSink>> m_sinks;

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

} // namespace hm::log
