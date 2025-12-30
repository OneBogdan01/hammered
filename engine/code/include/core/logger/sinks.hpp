#pragma once
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
struct LogMessage
{
  Level level;
  std::string loggerName;
  std::string payLoad;
  time_point timestamp;
  std::thread::id threadId;

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
template<typename Mutex>
struct ConsoleSink : BaseSink
{
  void Sink(const LogMessage& msg) override
  {
    std::scoped_lock lock(m_mutex);
    std::println("{}", msg.payLoad);
  }

  void Flush() override
  {
    std::scoped_lock lock(m_mutex);
    std::cout.flush();
  }

 private:
  Mutex m_mutex;
};

template<typename Mutex>
struct FileSink : BaseSink
{
  explicit FileSink(const std::string& fileName) : m_file(fileName) {}

  void Sink(const LogMessage& msg) override
  {
    std::scoped_lock lock(m_mutex);
    m_file << std::format("{}\n", msg.payLoad);
  }

  void Flush() override
  {
    std::scoped_lock lock(m_mutex);
    m_file.flush();
  }

 private:
  std::ofstream m_file;
  Mutex m_mutex;
};
using ConsoleSinkMt = ConsoleSink<std::mutex>;
using ConsoleSinkSt = ConsoleSink<NullMutex>;
using FileSinkMt = FileSink<std::mutex>;
using FileSinkSt = FileSink<NullMutex>;

} // namespace hm::log
