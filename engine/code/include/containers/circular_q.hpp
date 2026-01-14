#pragma once
#include <vector>

namespace hm::containers
{
// from
// https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/details/circular_q.h

template<typename T>
class CircularQueue
{
 private:
  u64 m_maxItems = 0;
  u64 m_overrunCount = 0;
  std::vector<T>::size_type m_head = 0;
  std::vector<T>::size_type m_tail = 0;
  std::vector<T> m_buffer;
};
} // namespace hm::containers
