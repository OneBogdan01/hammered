#pragma once
#include <format>
namespace hm::log {

template <typename... Args>
void info(std::format_string<Args...> fmt, Args&&... args) {
    std::println("[info]  {}", std::format(fmt, std::forward<Args>(args)...));
}

// stderr is unbuffered, so it writes immediately
template <typename... Args>
void warn(std::format_string<Args...> fmt, Args&&... args) {
    std::println(stderr, "[warn]  {}", std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void error(std::format_string<Args...> fmt, Args&&... args) {
    std::println(stderr, "[error] {}", std::format(fmt, std::forward<Args>(args)...));
}
} // namespace hm::log