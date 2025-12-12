#pragma once
#include <mutex>
#include <queue>
namespace hm
{
// based initially on
// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
class ThreadPool
{
 public:
  ThreadPool(u64 threadCount = 0);
  ~ThreadPool();
  HM_NON_COPYABLE(ThreadPool);
  void QueueJob(const std::function<void()>& job);
  void Shutdown();
  bool Busy();

 private:
  std::vector<std::thread> m_threads;
  std::atomic<u32> m_activeJobs;
  void ThreadLoop();
  bool should_terminate = false; // Tells threads to stop looking for jobs
  std::mutex queue_mutex;        // Prevents data races to the job queue
  std::condition_variable
      mutex_condition; // Allows threads to wait on new jobs or termination
  std::queue<std::function<void()>> jobs;
};
} // namespace hm
