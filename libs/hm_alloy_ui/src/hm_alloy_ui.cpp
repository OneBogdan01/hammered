#include "hm_alloy_ui.hpp"
#include "app.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <SDL3/SDL.h>
#include <cassert>

void hm::alloy ::AlloyUiPlugin::build(App& app) {
    app.add_systems(Schedule::Startup, [](App& a) {
        auto* gpu_device = SDL_CreateGPUDevice(
            SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true,
            NULL);

        if (!gpu_device) {
            SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
            return;
        }

        a.world().set<RendererHandle>({.gpu_device = gpu_device});
        SDL_Log("GPU Device created successfully");

        const auto* window_handle = a.world().try_get<WindowHandle>();
        if (window_handle == nullptr || window_handle->window == nullptr) {
            SDL_Log("There is no window handle");
            assert(false);
            return;
        }
        SDL_ClaimWindowForGPUDevice(gpu_device, window_handle->window);
    });

    app.add_systems(Schedule::Shutdown, [](App& a) {
        auto& world = a.world();
        if (!world.has<RendererHandle>())
            return;

        auto& gpu_handle = world.ensure<RendererHandle>();
        const auto* window_handle = world.try_get<WindowHandle>();

        if (window_handle == nullptr)
            return;

        SDL_ReleaseWindowFromGPUDevice(gpu_handle.gpu_device, window_handle->window);
        SDL_DestroyGPUDevice(gpu_handle.gpu_device);
        gpu_handle.gpu_device = nullptr;

        SDL_Log("GPU Device destroyed");
    });
}