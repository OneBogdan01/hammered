#pragma once
#include <SDL3/SDL_render.h>

namespace hm {
struct RendererHandle {
    SDL_Renderer* renderer{nullptr};
};
} // namespace hm