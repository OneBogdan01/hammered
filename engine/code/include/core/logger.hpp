#pragma once
#include "logger/base_logger.hpp"
#include "logger/sinks.hpp"

#include <format>

namespace hm::log
{
inline std::shared_ptr<LoggerMt>& GetGlobalLogger()
{
  static std::shared_ptr<LoggerMt> instance;
  return instance;
}
inline void SetDefaultLogger(std::shared_ptr<LoggerMt> logger)
{
  GetGlobalLogger() = std::move(logger);
}

inline std::shared_ptr<LoggerMt> GetDefaultLogger()
{
  return GetGlobalLogger();
}

inline std::shared_ptr<LoggerMt> CreateConsoleLogger()
{
  auto logger = std::make_shared<LoggerMt>();
  logger->sinks.push_back(std::make_shared<ConsoleSinkMt>());
  return logger;
}

inline std::shared_ptr<LoggerMt> CreateFileLogger(const std::string& path)
{
  auto logger = std::make_shared<LoggerMt>();
  logger->sinks.push_back(std::make_shared<ConsoleSinkMt>());
  logger->sinks.push_back(std::make_shared<FileSinkMt>(path));
  return logger;
}

template<typename... T>
void Info(std::format_string<T...> fs, T&&... args)
{
  if (auto logger = GetDefaultLogger())
    logger->Info(fs, std::forward<T>(args)...);
}

template<typename... T>
void Error(std::format_string<T...> fs, T&&... args)
{
  if (auto logger = GetDefaultLogger())
    logger->Error(fs, std::forward<T>(args)...);
}

template<typename... T>
void Warning(std::format_string<T...> fs, T&&... args)
{
  if (auto logger = GetDefaultLogger())
    logger->Warning(fs, std::forward<T>(args)...);
}

template<typename... T>
void Debug(std::format_string<T...> fs, T&&... args)
{
  if (auto logger = GetDefaultLogger())
    logger->Debug(fs, std::forward<T>(args)...);
}

inline void Flush()
{
  if (auto logger = GetDefaultLogger())
    logger->Flush();
}

} // namespace hm::log
