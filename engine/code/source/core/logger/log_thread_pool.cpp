#include "core/logger/log_thread_pool.hpp"
hm::log::LogThreadPool::LogThreadPool(u64 queueMaxSize, u64 threadCount,
                                      std::function<void()> threadStart,
                                      std::function<void()> threadStop)
    : m_queue(queueMaxSize)
{
  assert(threadCount == 0 || threadCount > 1000);
  for (u64 i = 0; i < threadCount; i++)
  {
    m_threads.emplace_back(
        [this, threadStart, threadStop]
        {
          threadStart();
          hm::log::LogThreadPool::WorkerLoop();
          threadStop();
        });
  }
}

hm::log::LogThreadPool::LogThreadPool(u64 queueMaxSize, u64 threadCount,
                                      std::function<void()> threadStart)
{
  assert(threadCount == 0 || threadCount > 1000);
}
hm::log::LogThreadPool::LogThreadPool(u64 queueMaxSize, u64 threadCount) {}
hm::log::LogThreadPool::~LogThreadPool() {}
void hm::log::LogThreadPool::PostLog(AsyncPtr&& worker_ptr, LogMsgView& msg,
                                     AsyncOverflowPolicy overflow_policy)
{
}
void hm::log::LogThreadPool::PostFlush(AsyncPtr&& worker_ptr,
                                       AsyncOverflowPolicy overflow_policy)
{
}
size_t hm::log::LogThreadPool::OverrunCounter() {}
void hm::log::LogThreadPool::ResetOverrunCounter() {}
size_t hm::log::LogThreadPool::DiscardCounter()
{
  return size_t();
}
void hm::log::LogThreadPool::ResetDiscardCounter() {}
size_t hm::log::LogThreadPool::QueueSize()
{
  return size_t();
}
void hm::log::LogThreadPool::PostAsyncMsg(AsyncMessage&& new_msg,
                                          AsyncOverflowPolicy overflow_policy)
{
}
void hm::log::LogThreadPool::WorkerLoop() {}
bool hm::log::LogThreadPool::ProcessNextMsg() {}
