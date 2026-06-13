#include "hm_gpu.hpp"
#include "app.hpp"
#include "hm_sdl.hpp"
#include "hm_window.hpp"

namespace hm {
class WindowPlugin;
GPUPlugin::GPUPlugin(const GPUConfig cfg) : m_cfg(cfg) {}
void GPUPlugin::build(App& app) {
    app.add_systems(Schedule::Startup, [this](App& a) {
        m_gpu_device = check_sdl(SDL_CreateGPUDevice(
            SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, true,
            nullptr));
        if (m_gpu_device == nullptr)
            return;
        log::info("GPU device created successfully");

        auto window_plugin = a.get_plugin_mutable<WindowPlugin>();
        if (!window_plugin) {
            log::error("GPUPlugin requires WindowPlugin to be added first");
            return;
        }
        WindowHandle window = window_plugin->get().get_window_handle();
        if (window == nullptr) {
            log::error("WindowPlugin has no valid window handle");
            return;
        }

        if (!check_sdl(SDL_ClaimWindowForGPUDevice(m_gpu_device, window)))
            return;

        if (!SDL_SetGPUSwapchainParameters(m_gpu_device, window,
                                           m_cfg.composition, m_cfg.present_mode)) {
            log::warn("Could not set requested swapchain params (staying on default vsync/SDR): {}",
                      SDL_GetError());
        }
    });

    app.add_systems(Schedule::Shutdown, [this](App& a) {
        if (m_gpu_device == nullptr)
            return;

        if (auto window_plugin = a.get_plugin_mutable<WindowPlugin>()) {
            if (const WindowHandle window = window_plugin->get().get_window_handle()) {
                SDL_ReleaseWindowFromGPUDevice(m_gpu_device, window);
            }
        }
        SDL_DestroyGPUDevice(m_gpu_device);
        m_gpu_device = nullptr;
        log::info("GPU device destroyed");
    });
}
} // namespace hm