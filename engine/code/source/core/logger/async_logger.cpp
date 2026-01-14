#include "core/logger/async_logger.hpp"
#include "core/logger/log_thread_pool.hpp"
namespace hm::log
{
AsyncLogger::AsyncLogger(
    std::string name,
    std::initializer_list<std::shared_ptr<BaseSink>> sinksList,
    std::weak_ptr<LogThreadPool> pool, AsyncOverflowPolicy policy)
    : AsyncLogger(std::move(name), sinksList.begin(), sinksList.end(),
                  std::move(pool), policy)
{
}
AsyncLogger::AsyncLogger(std::string name, std::shared_ptr<BaseSink> sink,
                         std::weak_ptr<LogThreadPool> pool,
                         AsyncOverflowPolicy policy)
    : AsyncLogger(std::move(name), {std::move(sink)}, std::move(pool), policy)
{
}
void AsyncLogger::Log(Level level, std::string_view msg)
{
  if (level < m_level)
    return;

  LogMsgView view {.level = level,
                   .loggerName = m_name,
                   .payload = msg,
                   .timestamp = clock::now(),
                   .threadId = std::this_thread::get_id()};

  auto pool = m_threadPool.lock();
  assert(pool);
  if (pool)
  {
    pool->PostLog(shared_from_this(), view, m_overflowPolicy);
  }
}
void AsyncLogger::Flush()
{
  auto pool = m_threadPool.lock();
  assert(pool);
  if (pool)
  {
    pool->PostFlush(shared_from_this(), m_overflowPolicy);
  }
}
void AsyncLogger::BackendSink(const LogMessage& msg)
{
  for (auto& sink : sinks)
  {
    sink->Sink(msg);
  }
  if (msg.level >= Level::Error)
  {
    BackendFlush();
  }
}
void AsyncLogger::BackendFlush()
{
  for (auto& sink : sinks)
  {
    sink->Flush();
  }
}
std::shared_ptr<AsyncLogger> AsyncLogger::Clone(std::string newName)
{
  auto cloned = std::make_shared<AsyncLogger>(*this);
  cloned->m_name = std::move(newName);
  return cloned;
}
} // namespace hm::log
