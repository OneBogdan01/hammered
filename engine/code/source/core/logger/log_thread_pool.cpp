#include "core/logger/log_thread_pool.hpp"

#include "core/logger/async_logger.hpp"
hm::log::LogThreadPool::LogThreadPool(u64 queueMaxSize, u64 threadCount,
                                      std::function<void()> threadStart,
                                      std::function<void()> threadStop)
    : m_queue(queueMaxSize)
{
  assert(threadCount != 0 && threadCount <= 1000);
  for (u64 i = 0; i < threadCount; i++)
  {
    m_threads.emplace_back(
        [this, threadStart, threadStop]
        {
          threadStart();
          WorkerLoop();
          threadStop();
        });
  }
}

hm::log::LogThreadPool::LogThreadPool(u64 queueMaxSize, u64 threadCount,
                                      std::function<void()> threadStart)
    : LogThreadPool(queueMaxSize, threadCount, std::move(threadStart),
                    []
                    {
                    })
{
}
hm::log::LogThreadPool::LogThreadPool(u64 queueMaxSize, u64 threadCount)
    : LogThreadPool(
          queueMaxSize, threadCount,
          []
          {
          },
          []
          {
          })
{
}
hm::log::LogThreadPool::~LogThreadPool()
{
  for (u64 i = 0; i < m_threads.size(); ++i)
  {
    PostAsyncMsg(AsyncMessage {AsyncMessageType::TERMINATE},
                 AsyncOverflowPolicy::BLOCK);
  }

  for (auto& t : m_threads)
  {
    if (t.joinable())
      t.join();
  }
}
void hm::log::LogThreadPool::PostLog(AsyncPtr&& worker_ptr, LogMsgView& msg,
                                     AsyncOverflowPolicy overflow_policy)
{
  AsyncMessage asyncMsg(std::move(worker_ptr), AsyncMessageType::LOG, msg);
  PostAsyncMsg(std::move(asyncMsg), overflow_policy);
}
void hm::log::LogThreadPool::PostFlush(AsyncPtr&& worker_ptr,
                                       AsyncOverflowPolicy overflow_policy)
{
  PostAsyncMsg(AsyncMessage(std::move(worker_ptr), AsyncMessageType::FLUSH),
               overflow_policy);
}
u64 hm::log::LogThreadPool::OverrunCounter()
{
  return m_queue.OverrunCount();
}
void hm::log::LogThreadPool::ResetOverrunCounter()
{
  m_queue.ResetDiscardCount();
}
u64 hm::log::LogThreadPool::DiscardCounter()
{
  return m_queue.DiscardCount();
}
void hm::log::LogThreadPool::ResetDiscardCounter()
{
  m_queue.ResetDiscardCount();
}
u64 hm::log::LogThreadPool::QueueSize()
{
  return m_queue.Size();
}
void hm::log::LogThreadPool::PostAsyncMsg(AsyncMessage&& new_msg,
                                          AsyncOverflowPolicy overflow_policy)
{
  switch (overflow_policy)
  {
    case AsyncOverflowPolicy::BLOCK:
      m_queue.Enqueue(std::move(new_msg));
      break;
    case AsyncOverflowPolicy::OVERRUN_OLDEST:
      m_queue.EnqueueNowait(std::move(new_msg));
      break;
    case AsyncOverflowPolicy::DISCARD_NEW:
      m_queue.EnqueueIfSpace(std::move(new_msg));
      break;
    default:
      assert(false);
      break;
  }
}
void hm::log::LogThreadPool::WorkerLoop()
{
  HM_ZONE_SCOPED_N("LogWorkerLoop");
  while (ProcessNextMsg())
  {
  }
}
bool hm::log::LogThreadPool::ProcessNextMsg()
{
  HM_ZONE_SCOPED_N("ProcessLogMsg");
  AsyncMessage incoming_msg;
  m_queue.Dequeue(incoming_msg);

  switch (incoming_msg.type)
  {
    case AsyncMessageType::LOG:
      incoming_msg.asyncLogger->BackendSink(incoming_msg.msg);
      return true;
    case AsyncMessageType::FLUSH:
      incoming_msg.asyncLogger->BackendFlush();
      return true;
    case AsyncMessageType::TERMINATE:
      return false;
  }
  return true;
}
