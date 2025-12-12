#include "core/thread_pool.hpp"

#include "utility/logger.hpp"

using namespace hm;
ThreadPool::ThreadPool(u64 threadCount)
{
  if (threadCount == 0)
  {
    // without the main thread and one for the OS
    const auto hwThreads = std::thread::hardware_concurrency();
    threadCount = hwThreads > 2 ? hwThreads - 2 : 1;
  }
  m_threads.reserve(threadCount);
  hm::log::Info("Thread pool initialized with {}", threadCount);

  for (size_t i = 0; i < threadCount; i++)
  {
    m_threads.emplace_back(&ThreadPool::ThreadLoop, this);
  }
}
ThreadPool::~ThreadPool()
{
  Shutdown();
}
void ThreadPool::ThreadLoop()
{
  while (true)
  {
    std::function<void()> job;
    {
      std::unique_lock lock(queue_mutex);
      mutex_condition.wait(lock,
                           [this]
                           {
                             return !jobs.empty() || should_terminate;
                           });
      if (should_terminate)
      {
        return;
      }
      job = jobs.front();
      jobs.pop();
      ++m_activeJobs;
    }
    job();
    --m_activeJobs;
  }
}
void ThreadPool::QueueJob(const std::function<void()>& job)
{
  {
    std::unique_lock lock(queue_mutex);
    jobs.push(job);
  }
  mutex_condition.notify_one();
}
void ThreadPool::Shutdown()
{
  {
    std::unique_lock lock(queue_mutex);
    should_terminate = true;
  }
  mutex_condition.notify_all();
  for (std::thread& active_thread : m_threads)
  {
    active_thread.join();
  }
  m_threads.clear();
}
bool ThreadPool::Busy()
{
  bool poolbusy;
  {
    std::unique_lock lock(queue_mutex);
    poolbusy = !jobs.empty() || m_activeJobs > 0;
  }
  return poolbusy;
}
