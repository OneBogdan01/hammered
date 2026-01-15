#pragma once
#include "base_logger.hpp"
#include "log_thread_pool.hpp"
#include "mutex.hpp"
namespace hm::log
{
class LogThreadPool;

class AsyncLogger : public std::enable_shared_from_this<AsyncLogger>,
                    public BaseLogger<NullMutex>
{
 public:
  using BaseLogger::Log;
  template<typename It>
  AsyncLogger(std::string name, It begin, It end,
              std::weak_ptr<LogThreadPool> pool,
              AsyncOverflowPolicy policy = AsyncOverflowPolicy::BLOCK)
      : m_threadPool(std::move(pool)), m_overflowPolicy(policy)
  {
    m_name = std::move(name);
    sinks.assign(begin, end);
  }

  AsyncLogger(std::string name,
              std::initializer_list<std::shared_ptr<BaseSink>> sinksList,
              std::weak_ptr<LogThreadPool> pool,
              AsyncOverflowPolicy policy = AsyncOverflowPolicy::BLOCK);

  AsyncLogger(std::string name, std::shared_ptr<BaseSink> sink,
              std::weak_ptr<LogThreadPool> pool,
              AsyncOverflowPolicy policy = AsyncOverflowPolicy::BLOCK);

  void Log(Level level, std::string_view msg) override;
  void Flush() override;

  void BackendSink(const LogMessage& msg);
  void BackendFlush();
  std::shared_ptr<AsyncLogger> Clone(std::string newName);

 private:
  friend class LogThreadPool;
  std::weak_ptr<LogThreadPool> m_threadPool;
  AsyncOverflowPolicy m_overflowPolicy {AsyncOverflowPolicy::BLOCK};
};
} // namespace hm::log
