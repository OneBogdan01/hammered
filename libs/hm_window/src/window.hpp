#pragma once
#include "hm_core.hpp"

namespace hm {
struct Window {
    u32 width = 0u;
    u32 height = 0u;
    std::string title{"Default Window Name"};
};
struct WindowConfig {
    std::string title = "Hammered Engine";
    i32 width = 800;
    i32 height = 600;
};

struct WindowHandle {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};
} // namespace hm
