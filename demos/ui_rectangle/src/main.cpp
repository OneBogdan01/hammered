#include "prelude.hpp"

#include "entry.hpp"
#include "hm_alloy_ui.hpp"
#include "renderer.hpp"
#include "window.hpp"

void hm_setup(hm::App& app) {
    using namespace hm;

    app.world().set<WindowConfig>({.title = "Rectangle UI", .width = 640, .height = 320});
    app.add_plugin<WindowPlugin>().add_plugin<AlloyUi>();

    app.add_systems(Schedule::Update, [](App& a) {
        auto& world = a.world();
        const auto* gpu_device = world.try_get<RendererHandle>();
        const auto* window_handle = world.try_get<WindowHandle>();
        if (!gpu_device || !window_handle)
            return;

        auto* cmdbuf = SDL_AcquireGPUCommandBuffer(gpu_device->gpu_device);
        if (cmdbuf == nullptr) {
            SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
            return;
        }

        SDL_GPUTexture* swapchainTexture;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window_handle->window, &swapchainTexture,
                                                   nullptr, nullptr)) {
            SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
            return;
        }

        if (swapchainTexture != nullptr) {
            SDL_GPUColorTargetInfo colorTargetInfo = {nullptr};
            colorTargetInfo.texture = swapchainTexture;
            auto t = SDL_GetTicks() / 1000.f;
            auto r = static_cast<float>((std::sin(t) + 1) / 2.0);
            auto g = static_cast<float>((std::sin(t / 2) + 1) / 2.0);
            auto b = static_cast<float>((std::sin(t * 2) + 1) / 2.0);
            colorTargetInfo.clear_color = SDL_FColor{r, g, b, 1.0f};
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass =
                SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, nullptr);
            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    });
}