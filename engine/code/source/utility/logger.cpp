#include "utility/logger.hpp"

void hm::log::ConsoleSink::Sink(const LogMessage& msg)
{
  std::println("{}", msg.payLoad);
}
void hm::log::ConsoleSink::Flush()
{
  std::cout.flush();
}
void hm::log::FileSink::Sink(const LogMessage& msg)
{
  output_file << std::format("{}\n", msg.payLoad);
}
void hm::log::FileSink::Flush()
{
  output_file.flush();
}

void hm::log::Logger::Log(Level level, std::string_view msg)
{
  if (level < m_level)
    return;
  auto now = clock::now();
  std::scoped_lock lock(m_mutex);
  buffer.emplace_back(LogMessage {.level = level,
                                  .loggerName = m_name,
                                  .payLoad = std::string(msg),
                                  .timestamp = now});
}
void hm::log::Logger::Log(Level level, std::string_view msg, time_point ts)
{
  if (level < m_level)
    return;
  std::scoped_lock lock(m_mutex);
  buffer.emplace_back(LogMessage {.level = level,
                                  .loggerName = m_name,
                                  .payLoad = std::string(msg),
                                  .timestamp = ts});
}
void hm::log::Logger::Flush()
{
  std::scoped_lock lock(m_mutex);
  // can be sorted by time
  std::sort(buffer.begin(), buffer.end());
  for (const auto& msg : buffer)
  {
    for (auto& sink : sinks)
    {
      sink->Sink(msg);
    }
  }
  buffer.clear();
  for (const auto& sink : sinks)
  {
    sink->Flush();
  }
}
