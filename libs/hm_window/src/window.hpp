#pragma once
#include "hm_core.hpp"

namespace hm {

struct WindowConfig {
    std::string title = "Hammered Engine";
    i32 width{800};
    i32 height{600};
};

struct WindowHandle {
    SDL_Window* window{nullptr};
};
} // namespace hm
