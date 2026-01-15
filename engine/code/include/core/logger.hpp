#pragma once
#include "logger/async_logger.hpp"
#include "logger/log_thread_pool.hpp"
#include "logger/base_logger.hpp"
#include "logger/sinks.hpp"

#include <format>

namespace hm::log
{

#define HM_LOGGER_TYPE 2

#if HM_LOGGER_TYPE == 0
using DefaultLogger = LoggerSt;
#elif HM_LOGGER_TYPE == 1
using DefaultLogger = LoggerMt;
#else
using DefaultLogger = AsyncLogger;
#endif
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
inline std::shared_ptr<DefaultLogger>& GetGlobalLogger()
{
  static std::shared_ptr<DefaultLogger> instance;
  return instance;
}

inline void SetDefaultLogger(std::shared_ptr<DefaultLogger> logger)
{
  GetGlobalLogger() = std::move(logger);
}

inline std::shared_ptr<DefaultLogger> GetDefaultLogger()
{
  return GetGlobalLogger();
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

inline std::shared_ptr<DefaultLogger> CreateFileLogger(
    const std::string& path,
    [[maybe_unused]] const std::string& name = "Global",
    [[maybe_unused]] AsyncOverflowPolicy policy = AsyncOverflowPolicy::BLOCK)
{
#if HM_LOGGER_TYPE == 0
  auto logger = std::make_shared<LoggerSt>();
  logger->sinks.push_back(std::make_shared<ConsoleSinkSt>());
  logger->sinks.push_back(std::make_shared<FileSinkSt>(path));
  return logger;

#elif HM_LOGGER_TYPE == 1
  auto logger = std::make_shared<LoggerMt>();
  logger->sinks.push_back(std::make_shared<ConsoleSinkMt>());
  logger->sinks.push_back(std::make_shared<FileSinkMt>(path));
  return logger;

#else
  auto& pool = GetGlobalThreadPool();
  if (!pool)
  {
    log::InitThreadPool(8192 * 5, 1);
  }
  return std::make_shared<AsyncLogger>(
      name,
      std::initializer_list<std::shared_ptr<BaseSink>> {
          std::make_shared<ConsoleSinkMt>(),
          std::make_shared<FileSinkMt>(path)},
      pool, policy);
#endif
}

inline std::shared_ptr<DefaultLogger> CreateConsoleLogger(
    [[maybe_unused]] const std::string& name = "Global",
    [[maybe_unused]] AsyncOverflowPolicy policy = AsyncOverflowPolicy::BLOCK)
{
#if HM_LOGGER_TYPE == 0
  auto logger = std::make_shared<LoggerSt>();
  logger->sinks.push_back(std::make_shared<ConsoleSinkSt>());
  return logger;

#elif HM_LOGGER_TYPE == 1
  auto logger = std::make_shared<LoggerMt>();
  logger->sinks.push_back(std::make_shared<ConsoleSinkMt>());
  return logger;

#else
  auto& pool = GetGlobalThreadPool();
  assert(pool && "Call InitThreadPool() first");
  return std::make_shared<AsyncLogger>(name, std::make_shared<ConsoleSinkMt>(),
                                       pool, policy);
#endif
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
