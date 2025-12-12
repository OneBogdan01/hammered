#pragma once

#include <string>

namespace hm
{
struct ProfilerSettings
{
  bool overwrite = true;
  bool use_timer = true;
  int32_t port = 8086;
  int32_t duration = 5;
  std::string profiler_path = "D:\\tracy\\tracy-profiler.exe";
  std::string capture_path = "D:\\tracy\\tracy-capture.exe";
  std::string output_path = "profile.tracy";
  std::string address = "127.0.0.1";
};
// This implementation is windows specific
class Profiler
{
 public:
  void display();
};

} // namespace hm
