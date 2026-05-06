#pragma once

#include "SDL3/SDL_gpu.h"
namespace hm {
struct RendererHandle {
    SDL_GPUDevice* gpu_device{nullptr};
};
}