#pragma once
#include <source_location>
#include "hm_log.hpp"
#include <SDL3/SDL.h>
namespace hm {
// SDL
// Window
using WindowHandle = SDL_Window*;
// structs and flags used
using fPoint64 = SDL_FPoint;
using fRect128 = SDL_FRect;
using WindowFlags = SDL_WindowFlags;
namespace window_flags {
inline constexpr WindowFlags RESIZABLE = SDL_WINDOW_RESIZABLE;
inline constexpr WindowFlags FULLSCREEN = SDL_WINDOW_FULLSCREEN;
inline constexpr WindowFlags BORDERLESS = SDL_WINDOW_BORDERLESS;
inline constexpr WindowFlags HIDDEN = SDL_WINDOW_HIDDEN;
} // namespace window_flags

// GPU
using GPUDeviceHandle = SDL_GPUDevice*;
using ShaderHandle = SDL_GPUShader*;
using ComputePipelineHandle = SDL_GPUComputePipeline*;
//Buffers
using BufferHandle = SDL_GPUBuffer*;
using SwapchainComposition = SDL_GPUSwapchainComposition;
using PresentMode = SDL_GPUPresentMode;

namespace present_mode {
inline constexpr PresentMode VSYNC = SDL_GPU_PRESENTMODE_VSYNC;
inline constexpr PresentMode IMMEDIATE = SDL_GPU_PRESENTMODE_IMMEDIATE;
inline constexpr PresentMode MAILBOX = SDL_GPU_PRESENTMODE_MAILBOX;
} // namespace present_mode

namespace swapchain_composition {
inline constexpr SwapchainComposition SDR = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
inline constexpr SwapchainComposition SDR_LINEAR = SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR;
inline constexpr SwapchainComposition HDR_EXT_LINEAR =
    SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR;
inline constexpr SwapchainComposition HDR10_ST2084 = SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084;
} // namespace swapchain_composition
[[nodiscard]] inline bool check_sdl(const bool ok,
                                    std::source_location loc = std::source_location::current()) {
    if (!ok)
        log::error("SDL [{}:{}]: {}", loc.file_name(), loc.line(), SDL_GetError());
    return ok;
}

template <typename T>
[[nodiscard]] T* check_sdl(T* ptr, std::source_location loc = std::source_location::current()) {
    if (ptr == nullptr)
        log::error("SDL [{}:{}]: {}", loc.file_name(), loc.line(), SDL_GetError());
    return ptr; // pointer flows through, so you keep the window handle
}
} // namespace hm