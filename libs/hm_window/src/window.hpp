#pragma once
#include "hm_core.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

namespace hm {

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
