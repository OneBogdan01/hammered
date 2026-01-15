#pragma once
#include "mutex.hpp"

#include <chrono>
#include <string>
#include <print>
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

using clock = std::chrono::steady_clock;
using time_point = clock::time_point;
struct LogMsgView
{
  Level level;
  std::string_view loggerName;
  std::string_view payload;
  time_point timestamp;
  std::thread::id threadId;
};
struct LogMessage
{
  Level level;
  std::string loggerName;
  std::string payload;
  time_point timestamp;
  std::thread::id threadId;

  LogMessage() = default;
  explicit LogMessage(const LogMsgView& view)
      : level(view.level),
        loggerName(view.loggerName),
        payload(view.payload),
        timestamp(view.timestamp),
        threadId(view.threadId)
  {
  }
  bool operator<(const LogMessage& other) const
  {
    return timestamp < other.timestamp;
  }
  operator LogMsgView() const
  {
    return {level, loggerName, payload, timestamp, threadId};
  }
};
struct BaseSink
{
  virtual ~BaseSink() = default;
  virtual void Sink(LogMsgView msg) = 0;
  virtual void Flush() = 0;
};
template<typename Mutex>
struct ConsoleSink : BaseSink
{
  void Sink(LogMsgView msg) override
  {
    HM_ZONE_SCOPED_N("ConsoleSink::Sink");
    std::scoped_lock lock(m_mutex);
    std::println("{}", msg.payload);
  }

  void Flush() override
  {
    HM_ZONE_SCOPED_N("ConsoleSink::Flush");

    std::scoped_lock lock(m_mutex);
    std::cout.flush();
  }

 private:
  HM_LOCKABLE_N(Mutex, m_mutex, "ConsoleSink");
};

template<typename Mutex>
struct FileSink : BaseSink
{
  explicit FileSink(const std::string& fileName) : m_file(fileName) {}

  void Sink(LogMsgView msg) override
  {
    HM_ZONE_SCOPED_N("FileSink::Sink");
    std::scoped_lock lock(m_mutex);
    m_file << std::format("{}\n", msg.payload);
  }

  void Flush() override
  {
    HM_ZONE_SCOPED_N("FileSink::Flush");
    std::scoped_lock lock(m_mutex);
    m_file.flush();
  }

 private:
  std::ofstream m_file;
  HM_LOCKABLE_N(Mutex, m_mutex, "ConsoleSink");
};
using ConsoleSinkMt = ConsoleSink<std::mutex>;
using ConsoleSinkSt = ConsoleSink<NullMutex>;
using FileSinkMt = FileSink<std::mutex>;
using FileSinkSt = FileSink<NullMutex>;

} // namespace hm::log
