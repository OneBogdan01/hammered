#include "prelude.hpp"

#include "entry.hpp"
#include "hm_alloy_ui.hpp"
#include "renderer.hpp"
#include "window.hpp"

static SDL_GPUGraphicsPipeline* pipeline;
static SDL_GPUBuffer* vertex_buffer;
struct PositionColorVertex {
    float x, y, z;
};
void hm_setup(hm::App& app) {
    using namespace hm;

    app.world().set<WindowConfig>({.title = "Rectangle UI", .width = 640, .height = 320});
    app.add_plugin<WindowPlugin>().add_plugin<AlloyUiPlugin>();

    app.add_systems(Schedule::Startup, [](App& a) {
        const auto* gpu_device = a.world().try_get<RendererHandle>();
        if (gpu_device == nullptr) {
            return;
        }

        // Create the shaders
        SDL_GPUShader* vertexShader =
            hm::LoadShader(gpu_device->gpu_device, "ui_canvas.vert", 0, 0, 0, 0);
        if (vertexShader == NULL) {
            SDL_Log("Failed to create vertex shader!");
            return;
        }

        SDL_GPUShader* fragmentShader =
            hm::LoadShader(gpu_device->gpu_device, "ui_canvas.frag", 0, 1, 0, 0);
        if (fragmentShader == NULL) {
            SDL_Log("Failed to create fragment shader!");
            return;
        }
        const auto* window_handle = a.world().try_get<WindowHandle>();
        if (window_handle == nullptr) {
            return;
        }
        SDL_GPUVertexBufferDescription buffer_desc{.slot = 0,
                                                   .pitch = sizeof(PositionColorVertex),
                                                   .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                                                   .instance_step_rate = 0};
        SDL_GPUColorTargetDescription target_desc{
            .format =
                SDL_GetGPUSwapchainTextureFormat(gpu_device->gpu_device, window_handle->window),
            .blend_state = SDL_GPUColorTargetBlendState{
        .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
        .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .color_blend_op        = SDL_GPU_BLENDOP_ADD,
        .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
        .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
        .color_write_mask      = 0xF,
        .enable_blend          = true,
    },};
        SDL_GPUVertexAttribute vertex_attrs[] = {{.location = 0,
                                                  .buffer_slot = 0,
                                                  .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                                                  .offset = 0}

        };
        // Create the pipeline
        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
            .vertex_shader = vertexShader,
            // This is set up to match the vertex shader layout!
            .fragment_shader = fragmentShader,
            .vertex_input_state =
                SDL_GPUVertexInputState{.vertex_buffer_descriptions = &buffer_desc,
                                        .num_vertex_buffers = 1,
                                        .vertex_attributes = vertex_attrs,
                                        .num_vertex_attributes = 1},
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .target_info = {
                .color_target_descriptions = &target_desc,
                .num_color_targets = 1,
            }};

        pipeline = SDL_CreateGPUGraphicsPipeline(gpu_device->gpu_device, &pipelineCreateInfo);
        if (pipeline == NULL) {
            SDL_Log("Failed to create pipeline!");
            return;
        }

        SDL_ReleaseGPUShader(gpu_device->gpu_device, vertexShader);
        SDL_ReleaseGPUShader(gpu_device->gpu_device, fragmentShader);

        // Create the vertex buffer
        SDL_GPUBufferCreateInfo buffer_create_info{.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                                   .size = sizeof(PositionColorVertex) * 3};
        vertex_buffer = SDL_CreateGPUBuffer(gpu_device->gpu_device, &buffer_create_info);

        // To get data into the vertex buffer, we have to use a transfer buffer
        SDL_GPUTransferBufferCreateInfo buffer_info{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                                    .size = sizeof(PositionColorVertex) * 3};
        auto* transferBuffer = SDL_CreateGPUTransferBuffer(gpu_device->gpu_device, &buffer_info);

        PositionColorVertex* transferData = static_cast<PositionColorVertex*>(
            SDL_MapGPUTransferBuffer(gpu_device->gpu_device, transferBuffer, false));

        transferData[0] = PositionColorVertex{-1, 1, 0};
        transferData[1] = PositionColorVertex{-1, -3, 0};
        transferData[2] = PositionColorVertex{3, 1, 0};

        SDL_UnmapGPUTransferBuffer(gpu_device->gpu_device, transferBuffer);

        // Upload the transfer data to the vertex buffer
        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(gpu_device->gpu_device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

        SDL_GPUTransferBufferLocation buffer_location{.transfer_buffer = transferBuffer,
                                                      .offset = 0};
        SDL_GPUBufferRegion buffer_region{
            .buffer = vertex_buffer, .offset = 0, .size = sizeof(PositionColorVertex) * 3};
        SDL_UploadToGPUBuffer(copyPass, &buffer_location, &buffer_region, false);

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
        SDL_ReleaseGPUTransferBuffer(gpu_device->gpu_device, transferBuffer);
    });
    app.add_systems(Schedule::Update, [](App& a) {
        const auto& world = a.world();
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
            const auto time = SDL_GetTicks() / 1000.f;
            // const auto r = static_cast<float>((std::sin(time) + 1) / 2.0);
            // const auto g = static_cast<float>((std::sin(time / 2) + 1) / 2.0);
            // const auto b = static_cast<float>((std::sin(time * 2) + 1) / 2.0);
            colorTargetInfo.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass =
                SDL_BeginGPURenderPass(cmdbuf, &colorTargetInfo, 1, nullptr);
            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
            struct WindowSize {
                f32 w;
                f32 h;
                f32 t;
            } window_size;
            int w, h;
            SDL_GetWindowSizeInPixels(window_handle->window, &w, &h);
            window_size = {.w = static_cast<float>(w), .h = static_cast<float>(h), .t = time};

            SDL_PushGPUFragmentUniformData(cmdbuf, 0, &window_size, sizeof(window_size));

            const SDL_GPUBufferBinding binding{.buffer = vertex_buffer, .offset = 0};
            SDL_BindGPUVertexBuffers(renderPass, 0, &binding, 1);
            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    });
}