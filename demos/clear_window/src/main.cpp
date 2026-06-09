#include "prelude.hpp"

#include "entry.hpp"

void hm_setup(hm::App& app) {
    app.add_plugin<hm::WindowPlugin>().add_plugin<hm::alloy::AlloyUiPlugin>();

    app.world().set(hm::WindowConfig{.title = "Clear Window", .width = 400, .height = 400});

    app.add_systems(hm::Schedule::Update, [](hm::App& a) {
        auto* render_handle = a.world().try_get<hm::RendererHandle>();
        auto* window_handle = a.world().try_get<hm::WindowHandle>();
        if (!render_handle || !render_handle->gpu_device || !window_handle->window)
            return;

        const auto t = SDL_GetTicks() / 1000.f;
        const auto r = (std::sin(t) + 1) / 2.0f;
        const auto g = (std::sin(t / 2) + 1) / 2.0f;
        const auto b = (std::sin(t * 2) + 1) / 2.0f;

        SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(render_handle->gpu_device);
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
