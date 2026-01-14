#pragma once
#include "async_logger.hpp"
#include "sinks.hpp"
#include "containers/mpmc_blocking_queue.hpp"

#include <functional>
#include <thread>
#include <vector>
namespace hm::log
{
// from https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/async_logger.h

enum class AsyncMessageType : u8
{
  LOG,
  FLUSH,
  TERMINATE
};
using AsyncPtr = std::shared_ptr<AsyncLogger>;

struct AsyncMessage
{
  AsyncMessageType type {AsyncMessageType::LOG};
  AsyncPtr asyncLogger;
  LogMessage msg;

  AsyncMessage() = default;
  ~AsyncMessage() = default;
  AsyncMessage(const AsyncMessage&) = delete;
  AsyncMessage& operator=(const AsyncMessage&) = delete;

  AsyncMessage(AsyncMessage&&) = default;
  AsyncMessage& operator=(AsyncMessage&&) = default;

  AsyncMessage(AsyncPtr&& worker, AsyncMessageType type, const LogMsgView& msg)
      : type(type), asyncLogger(std::move(worker)), msg(msg)
  {
  }
  AsyncMessage(AsyncPtr&& worker, AsyncMessageType type)
      : type(type), asyncLogger(std::move(worker)), msg()
  {
  }
  explicit AsyncMessage(AsyncMessageType type) : AsyncMessage(nullptr, type) {};
};
class LogThreadPool
{
 public:
  LogThreadPool(u64 queueMaxSize, u64 threadCount,
                std::function<void()> threadStart,
                std::function<void()> threadStop);
  LogThreadPool(u64 queueMaxSize, u64 threadCount,
                std::function<void()> threadStart);
  LogThreadPool(u64 queueMaxSize, u64 threadCount);
  ~LogThreadPool();

  LogThreadPool(const LogThreadPool&) = delete;
  LogThreadPool& operator=(LogThreadPool&) = delete;

  void PostLog(AsyncPtr&& worker_ptr, LogMsgView& msg,
               AsyncOverflowPolicy overflow_policy);
  void PostFlush(AsyncPtr&& worker_ptr, AsyncOverflowPolicy overflow_policy);
  size_t OverrunCounter();
  void ResetOverrunCounter();
  size_t DiscardCounter();
  void ResetDiscardCounter();
  size_t QueueSize();

 private:
  containers::MPMCBlockingQueue<AsyncMessage> m_queue;

  std::vector<std::thread> m_threads;

  void PostAsyncMsg(AsyncMessage&& new_msg,
                    AsyncOverflowPolicy overflow_policy);
  void WorkerLoop();

  // return true while no terminate msg was received
  bool ProcessNextMsg();
};
} // namespace hm::log
