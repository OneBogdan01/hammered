#include "command_buffer_ui.hpp"
#include "prelude.hpp"

#include "entry.hpp"
#include "hm_alloy_ui.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <algorithm>

static SDL_GPUGraphicsPipeline* pipeline;
static SDL_GPUBuffer* vertex_buffer;
struct Vertex {
    float x, y, z;
};
void hm_setup(hm::App& app) {
    using namespace hm;
    using namespace hm::alloy;

    app.world().set<WindowConfig>({.title = "Alloy Buffered Shapes", .width = 640, .height = 320});
    app.add_plugin<WindowPlugin>().add_plugin<AlloyUiPlugin>();

    app.add_systems(Schedule::Startup, [](App& a) {
        const auto* gpu_device = a.world().try_get<RendererHandle>();
        if (gpu_device == nullptr) {
            return;
        }
        // Create the shaders
        SDL_GPUShader* vertexShader =
            LoadShader(gpu_device->gpu_device, "ui_canvas.vert", 0, 0, 0, 0);
        if (vertexShader == NULL) {
            SDL_Log("Failed to create vertex shader!");
            return;
        }

        SDL_GPUShader* fragmentShader =
            LoadShader(gpu_device->gpu_device, "ui_canvas.frag", 0, 1, 1, 0);
        if (fragmentShader == NULL) {
            SDL_Log("Failed to create fragment shader!");
            return;
        }
        const auto* window_handle = a.world().try_get<WindowHandle>();
        if (window_handle == nullptr) {
            return;
        }
        SDL_GPUVertexBufferDescription buffer_desc{.slot = 0,
                                                   .pitch = sizeof(Vertex),
                                                   .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                                                   .instance_step_rate = 0};
        SDL_GPUColorTargetDescription target_desc{
            .format =
                SDL_GetGPUSwapchainTextureFormat(gpu_device->gpu_device, window_handle->window),
            .blend_state =
                SDL_GPUColorTargetBlendState{
                    .enable_blend = false,
                },
        };
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
                                                   .size = sizeof(Vertex) * 3};
        vertex_buffer = SDL_CreateGPUBuffer(gpu_device->gpu_device, &buffer_create_info);
        // prepare the buffer for UI
        {
            auto& ui_buffers{a.world().ensure<UIRenderResources>()};
            SDL_GPUTransferBufferCreateInfo ui_transfer_info{
                .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                .size = MAX_NUMBER_UI_SHAPES * sizeof(UICommand)};
            ui_buffers.transfer_buffer =
                SDL_CreateGPUTransferBuffer(gpu_device->gpu_device, &ui_transfer_info);
            SDL_GPUBufferCreateInfo ui_buffer_info{
                .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
                .size = MAX_NUMBER_UI_SHAPES * sizeof(UICommand)};
            ui_buffers.storage_buffer =
                SDL_CreateGPUBuffer(gpu_device->gpu_device, &ui_buffer_info);
        }
        // To get data into the vertex buffer, we have to use a transfer buffer
        SDL_GPUTransferBufferCreateInfo buffer_info{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                                    .size = sizeof(Vertex) * 3};
        auto* transferBuffer = SDL_CreateGPUTransferBuffer(gpu_device->gpu_device, &buffer_info);

        Vertex* transferData = static_cast<Vertex*>(
            SDL_MapGPUTransferBuffer(gpu_device->gpu_device, transferBuffer, false));

        transferData[0] = Vertex{-1, 1, 0};
        transferData[1] = Vertex{-1, -3, 0};
        transferData[2] = Vertex{3, 1, 0};

        SDL_UnmapGPUTransferBuffer(gpu_device->gpu_device, transferBuffer);

        // Upload the transfer data to the vertex buffer
        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(gpu_device->gpu_device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

        SDL_GPUTransferBufferLocation buffer_location{.transfer_buffer = transferBuffer,
                                                      .offset = 0};
        SDL_GPUBufferRegion buffer_region{
            .buffer = vertex_buffer, .offset = 0, .size = sizeof(Vertex) * 3};
        SDL_UploadToGPUBuffer(copyPass, &buffer_location, &buffer_region, false);

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
        SDL_ReleaseGPUTransferBuffer(gpu_device->gpu_device, transferBuffer);

        // Some interesting shapes being added to the buffer
        auto& cmd{a.world().get_mut<UICommandBuffer>()};

        cmd.add_circle(Circle{{300.0f, 50.0f}, 50.0f}, colors::u8::WHITE);
        cmd.add_rect(Rect{{200.0f, 50.0f, 400.0f, 150.0f}}, colors::u8::RED);
        cmd.add_line(Line{{50.0f, 200.0f}, {500.0f, 250.0f}}, colors::u8::CYAN);
        cmd.add_circle(Circle{{300.0f, 50.0f}, 50.0f}, colors::u8::WHITE);

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

        SDL_GPUTexture* swapchain_texture;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window_handle->window,
                                                   &swapchain_texture, nullptr, nullptr)) {
            SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
            return;
        }

        if (swapchain_texture != nullptr) {
            // ui update!
            auto& ui_render{a.world().get<UIRenderResources>()};
            auto& commands{a.world().get_mut<UICommandBuffer>().get_commands()};
            const auto time = SDL_GetTicks() / 1000.f;

            for (auto i=0; i<commands.size(); i++) {
                auto& shape{commands[i]};
                const auto r = static_cast<u8>(((std::sin(time+i) + 1.0f) / 2.0) * 255);
            const auto g = static_cast<u8>(((std::sin(time / 2) + 1) / 2.0) * 255);
            const auto b = static_cast<u8>(((std::sin(time * 2) + 1) / 2.0) * 255);
                switch (shape.type) {

                case ShapeType::Circle:
                    shape.color = uColor32{r, g, b, 255};
                    shape.circle.radius = (std::sin(time * 2 + 29) + 1) / 2.0f * 10.0f + 10.0f;
                    shape.circle.center.x = (std::cos(time * i) + 1) / 2.0f * 200.0f+100*i;
                    shape.circle.center.y = (std::sin(time * i) + 1) / 2.0f * 200.0f+20*i;
                    break;
                case ShapeType::Line:
                    shape.line.a.x = shape.line.b.x + std::cos(time) * 10;
                    shape.line.a.y = shape.line.b.y + std::sin(time*2) * 30;
                    break;
                case ShapeType::Rect:
                    shape.rect.rect.w = (std::sin(time ) + 1) / 2.0 * 400;
                    shape.rect.rect.h = (std::cos(time * 3) + 1) / 2.0f * 150;
                    shape.color.a = std::clamp<u8>((std::cos(time * 5) + 1) / 2.0f * 255u,150,255u);

                    break;
                }
            }
            {
                // Build sprite instance transfer
                auto* ptr = static_cast<UICommand*>(SDL_MapGPUTransferBuffer(
                    gpu_device->gpu_device, ui_render.transfer_buffer, true));

                const u32 count = std::min(commands.size(), MAX_NUMBER_UI_SHAPES);

                SDL_memcpy(ptr, commands.data(), count * sizeof(UICommand));

                SDL_UnmapGPUTransferBuffer(gpu_device->gpu_device, ui_render.transfer_buffer);

                // Upload data
                SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdbuf);
                const SDL_GPUTransferBufferLocation src{
                    .transfer_buffer = ui_render.transfer_buffer,
                    .offset = 0,
                };

                const SDL_GPUBufferRegion dst{
                    .buffer = ui_render.storage_buffer,
                    .offset = 0,
                    .size = count * sizeof(UICommand),
                };

                SDL_UploadToGPUBuffer(copyPass, &src, &dst, true);
                SDL_EndGPUCopyPass(copyPass);
            }

            SDL_GPUColorTargetInfo color_target_info = {nullptr};
            color_target_info.texture = swapchain_texture;

            color_target_info.clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f};
            color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
            color_target_info.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* render_pass =
                SDL_BeginGPURenderPass(cmdbuf, &color_target_info, 1, nullptr);
            SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
            struct WindowSize {
                f32 w;
                f32 h;
                f32 t;
                u32 count;
                fColor128 color;
            } window_size;
            int w, h;
            SDL_GetWindowSizeInPixels(window_handle->window, &w, &h);

            window_size = {.w = static_cast<float>(w),
                           .h = static_cast<float>(h),
                           .t = time,
                           .count = static_cast<u32>(commands.size()),
                           .color = hm::alloy::colors::f128::GRAY};

            SDL_PushGPUFragmentUniformData(cmdbuf, 0, &window_size, sizeof(window_size));
            // takes an array
            SDL_GPUBuffer* storage_buffers[] = {ui_render.storage_buffer};
            SDL_BindGPUFragmentStorageBuffers(render_pass, 0, storage_buffers, 1);
            const SDL_GPUBufferBinding binding{.buffer = vertex_buffer, .offset = 0};
            SDL_BindGPUVertexBuffers(render_pass, 0, &binding, 1);
            SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

            SDL_EndGPURenderPass(render_pass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    });
}