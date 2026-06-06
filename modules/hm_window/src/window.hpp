#pragma once
#include <SDL3/SDL_video.h>
namespace hm {

struct WindowConfig {
    std::string title = "Hammered Engine";
    i32 width{800};
    i32 height{600};
    bool vsync{true};
};

struct WindowHandle {
    SDL_Window* window{nullptr};
};


} // namespace hm
