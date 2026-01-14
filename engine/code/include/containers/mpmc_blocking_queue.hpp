#pragma once
#include "circular_q.hpp"

#include <atomic>
#include <condition_variable>
namespace hm::containers
{
// from
// https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/details/mpmc_blocking_q.h
// this is a wrapper around a circular queue that makes it thread safe for
// multiple consumers and multiple producers
template<typename T>
class MPMCBlockingQueue
{
 public:
  explicit MPMCBlockingQueue(u64 capacity) : m_queue(capacity) {}

  // try to enqueue and block if no room left
  void Enqueue(T&& item)
  {
    {
      std::unique_lock lock(m_queueMutex);
      m_popCV.wait(lock,
                   [this]
                   {
                     return !this->m_queue.full();
                   });
      m_queue.push_back(std::move(item));
    }
    m_pushCV.notify_one();
  }

  // enqueue immediately. overrun oldest message in the queue if no room left.
  void EnqueueNowait(T&& item)
  {
    {
      std::unique_lock lock(m_queueMutex);
      m_queue.push_back(std::move(item));
    }
    m_pushCV.notify_one();
  }

  void EnqueueRoom(T&& item)
  {
    bool pushed = false;
    {
      std::unique_lock lock(m_queueMutex);
      if (!m_queue.full())
      {
        m_queue.push_back(std::move(item));
        pushed = true;
      }
    }

    if (pushed)
    {
      m_pushCV.notify_one();
    }
    else
    {
      ++m_discardCounter;
    }
  }

  // dequeue with a timeout.
  // Return true, if succeeded dequeue item, false otherwise
  bool DequeueFor(T& popped_item, std::chrono::milliseconds wait_duration)
  {
    {
      std::unique_lock lock(m_queueMutex);
      if (!m_pushCV.wait_for(lock, wait_duration,
                             [this]
                             {
                               return !this->m_queue.empty();
                             }))
      {
        return false;
      }
      popped_item = std::move(m_queue.front());
      m_queue.pop_front();
    }
    m_popCV.notify_one();
    return true;
  }

  // blocking dequeue without a timeout.
  void Dequeue(T& popped_item)
  {
    {
      std::unique_lock lock(m_queueMutex);
      m_pushCV.wait(lock,
                    [this]
                    {
                      return !this->m_queue.empty();
                    });
      popped_item = std::move(m_queue.front());
      m_queue.pop_front();
    }
    m_popCV.notify_one();
  }

  size_t OverrunCount()
  {
    std::scoped_lock lock(m_queueMutex);
    return m_queue.overrun_counter();
  }

  size_t DiscardCount()
  {
    return m_discardCounter.load(std::memory_order_relaxed);
  }

  size_t Size()
  {
    std::scoped_lock lock(m_queueMutex);
    return m_queue.size();
  }

  void ResetOverrunCount()
  {
    std::scoped_lock lock(m_queueMutex);
    m_queue.reset_overrun_counter();
  }

  void ResetDiscardCount()
  {
    m_discardCounter.store(0, std::memory_order_relaxed);
  }

 private:
  std::mutex m_queueMutex;
  std::condition_variable m_pushCV;
  std::condition_variable m_popCV;
  CircularQueue<T> m_queue;
  std::atomic<size_t> m_discardCounter {0};
};
} // namespace hm::containers
