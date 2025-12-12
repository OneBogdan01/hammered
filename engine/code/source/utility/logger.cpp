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

  // std::scoped_lock lock(m_mutex);
  for (const auto& sink : m_sinks)
  {
    sink->Sink({level, m_name, msg});
  }
}
void hm::log::Logger::Flush()
{
  // std::lock_guard lock(m_mutex);
  for (auto& sink : m_sinks)
  {
    sink->Flush();
  }
}
