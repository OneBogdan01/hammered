#pragma once
#include <cassert>
#include <vector>

namespace hm::containers
{
// from
// https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/details/circular_q.h

template<typename T>
class CircularQueue
{
 public:
  explicit CircularQueue(u64 maxItems)
      : m_maxItems(maxItems + 1),
        m_buffer(m_maxItems) // one item reserved as marker for full queue
  {
  }
  CircularQueue(CircularQueue&& other) noexcept
  {
    CopyMoveable(std::move(other));
  }

  CircularQueue& operator=(CircularQueue&& other) noexcept
  {
    CopyMoveable(std::move(other));
    return *this;
  }
  void PushBack(T&& item)
  {
    if (m_maxItems > 0)
    {
      m_buffer[m_tail] = std::move(item);
      m_tail = (m_tail + 1) % m_maxItems;
      if (m_tail == m_head)
      {
        m_head = (m_head + 1) % m_maxItems;
        ++m_overrunCount;
      }
    }
  }
  // Return reference to the front item.
  // If there are no elements in the container, the behavior is undefined.
  const T& Front() const { return m_buffer[m_head]; }

  T& Front() { return m_buffer[m_head]; }

  // Return number of elements actually stored
  size_t Size() const
  {
    if (m_tail >= m_head)
    {
      return m_tail - m_head;
    }
    return m_maxItems - (m_head - m_tail);
  }

  // Return const reference to item by index.
  const T& At(size_t i) const
  {
    assert(i < size());
    return m_buffer[(m_head + i) % m_maxItems];
  }

  // Pop item from front.
  // If there are no elements in the container, the behavior is undefined.
  void PopFront() { m_head = (m_head + 1) % m_maxItems; }

  bool Empty() const { return m_tail == m_head; }

  bool Full() const
  {
    // head is ahead of the tail by 1
    if (m_maxItems > 0)
    {
      return ((m_tail + 1) % m_maxItems) == m_head;
    }
    return false;
  }

  size_t overrun_counter() const { return m_overrunCount; }

  void reset_overrun_counter() { m_overrunCount = 0; }

 private:
  void CopyMoveable(CircularQueue&& other)
  {
    m_maxItems = other.m_maxItems;
    m_head = other.m_head;
    m_tail = other.m_tail;
    m_overrunCount = other.m_overrunCount;
    m_buffer = std::move(other.m_buffer);

    // put &&other in disabled, but valid state
    other.m_maxItems = 0;
    other.m_head = other.m_tail = 0;
    other.m_overrunCount = 0;
  }
  u64 m_maxItems = 0;
  u64 m_overrunCount = 0;
  u64 m_head = 0;
  u64 m_tail = 0;
  std::vector<T> m_buffer;
};
} // namespace hm::containers
