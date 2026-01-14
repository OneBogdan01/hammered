#pragma once
#include "logger/async_logger.hpp"
#include "logger/log_thread_pool.hpp"
#include "logger/base_logger.hpp"
#include "logger/sinks.hpp"

#include <format>

namespace hm::log
{

inline std::shared_ptr<LogThreadPool>& GetGlobalThreadPool()
{
  static std::shared_ptr<LogThreadPool> instance;
  return instance;
}

inline void InitThreadPool(u64 queueSize = 8192, u64 threadCount = 1)
{
  GetGlobalThreadPool() =
      std::make_shared<LogThreadPool>(queueSize, threadCount);
}

inline void ShutdownThreadPool()
{
  GetGlobalThreadPool().reset();
}
//
// inline std::shared_ptr<LoggerMt>& GetGlobalLogger()
//{
//  static std::shared_ptr<LoggerMt> instance;
//  return instance;
//}
// inline void SetDefaultLogger(std::shared_ptr<LoggerMt> logger)
//{
//  GetGlobalLogger() = std::move(logger);
//}
//
// inline std::shared_ptr<LoggerMt> GetDefaultLogger()
//{
//  return GetGlobalLogger();
//}
inline std::shared_ptr<AsyncLogger>& GetGlobalLogger()
{
  static std::shared_ptr<AsyncLogger> instance;
  return instance;
}

inline void SetDefaultLogger(std::shared_ptr<AsyncLogger> logger)
{
  GetGlobalLogger() = std::move(logger);
}

inline std::shared_ptr<AsyncLogger> GetDefaultLogger()
{
  return GetGlobalLogger();
}

// Factory - async logger with file + console
inline std::shared_ptr<AsyncLogger> CreateAsyncFileLogger(
    const std::string& path, const std::string& name = "Global",
    AsyncOverflowPolicy policy = AsyncOverflowPolicy::OVERRUN_OLDEST)
{
  auto& pool = GetGlobalThreadPool();
  assert(pool);

  return std::make_shared<AsyncLogger>(
      name,
      std::initializer_list<std::shared_ptr<BaseSink>> {
          std::make_shared<FileSinkMt>(path),
          std::make_shared<ConsoleSinkMt>()},
      pool, policy);
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
inline void Shutdown()
{
  Flush();
  GetGlobalLogger().reset();
  ShutdownThreadPool();
}

} // namespace hm::log
