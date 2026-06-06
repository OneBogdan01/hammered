#include "command_buffer_ui.hpp"
#include "prelude.hpp"

#include "entry.hpp"
#include "hm_alloy_ui.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <algorithm>

// TODO use some kind ECS to kill the global state. Still fine for this small demo.
static SDL_GPUGraphicsPipeline* pipeline;
static SDL_GPUBuffer* vertex_buffer;

struct Vertex {
    float x, y, z;
};

static bool create_ui_pipeline(SDL_GPUDevice* device, SDL_Window* window) {
    SDL_GPUShader* vertex_shader = hm::alloy::LoadShader(device, "ui_canvas.vert", 0, 0, 0, 0);
    if (vertex_shader == nullptr) {
        SDL_Log("Failed to create vertex shader!");
        return false;
    }

    SDL_GPUShader* fragment_shader = hm::alloy::LoadShader(device, "ui_canvas.frag", 0, 1, 1, 0);
    if (fragment_shader == nullptr) {
        SDL_Log("Failed to create fragment shader!");
        return false;
    }

    SDL_GPUVertexBufferDescription buffer_desc{.slot = 0,
                                               .pitch = sizeof(Vertex),
                                               .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                                               .instance_step_rate = 0};
    SDL_GPUColorTargetDescription target_desc{
        .format = SDL_GetGPUSwapchainTextureFormat(device, window),
        .blend_state =
            SDL_GPUColorTargetBlendState{
                .enable_blend = false,
            },
    };
    SDL_GPUVertexAttribute vertex_attrs[] = {{.location = 0,
                                              .buffer_slot = 0,
                                              .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                                              .offset = 0}};

    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .vertex_input_state = SDL_GPUVertexInputState{.vertex_buffer_descriptions = &buffer_desc,
                                                      .num_vertex_buffers = 1,
                                                      .vertex_attributes = vertex_attrs,
                                                      .num_vertex_attributes = 1},
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = {
            .color_target_descriptions = &target_desc,
            .num_color_targets = 1,
        }};

    pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_create_info);

    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, fragment_shader);

    if (pipeline == nullptr) {
        SDL_Log("Failed to create pipeline!");
        return false;
    }
    return true;
}

// Creates the fullscreen-triangle vertex buffer and the UI shape storage buffers.
static void create_ui_buffers(hm::App& a, SDL_GPUDevice* device) {
    using namespace hm;

    SDL_GPUBufferCreateInfo vbuf_info{.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                      .size = sizeof(Vertex) * 3};
    vertex_buffer = SDL_CreateGPUBuffer(device, &vbuf_info);

    const u32 capacity = alloy::MAX_NUMBER_UI_SHAPES;

    auto& ui_buffers{a.world().ensure<alloy::UIRenderResources>()};
    SDL_GPUTransferBufferCreateInfo ui_transfer_info{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                                     .size = capacity * sizeof(alloy::UICommand)};
    ui_buffers.transfer_buffer = SDL_CreateGPUTransferBuffer(device, &ui_transfer_info);

    SDL_GPUBufferCreateInfo ui_buffer_info{.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
                                           .size = capacity * sizeof(alloy::UICommand)};
    ui_buffers.storage_buffer = SDL_CreateGPUBuffer(device, &ui_buffer_info);
}

// Uploads the single oversized triangle that covers the whole screen.
static void upload_fullscreen_triangle(SDL_GPUDevice* device) {
    SDL_GPUTransferBufferCreateInfo buffer_info{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                                .size = sizeof(Vertex) * 3};
    auto* transfer_buffer = SDL_CreateGPUTransferBuffer(device, &buffer_info);

    Vertex* transfer_data =
        static_cast<Vertex*>(SDL_MapGPUTransferBuffer(device, transfer_buffer, false));
    transfer_data[0] = Vertex{-1, 1, 0};
    transfer_data[1] = Vertex{-1, -3, 0};
    transfer_data[2] = Vertex{3, 1, 0};
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    SDL_GPUCommandBuffer* upload_cmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_cmd);

    SDL_GPUTransferBufferLocation location{.transfer_buffer = transfer_buffer, .offset = 0};
    SDL_GPUBufferRegion region{.buffer = vertex_buffer, .offset = 0, .size = sizeof(Vertex) * 3};
    SDL_UploadToGPUBuffer(copy_pass, &location, &region, false);

    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(upload_cmd);
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
}


void hm_setup(hm::App& app) {
    using namespace hm;
    using namespace hm::alloy;

    app.world().set<WindowConfig>({.title = "Tiled Shapes", .width = 640, .height = 320});
    app.add_plugin<WindowPlugin>().add_plugin<AlloyUiPlugin>();

    app.add_systems(Schedule::Startup, [](App& a) {
        // Seed the command buffer with a few shapes.
        {
            auto& cmd{a.world().get_mut<UICommandBuffer>()};
            cmd.add_circle(Circle{{300.0f, 50.0f}, 50.0f}, colors::u8::WHITE);
            cmd.add_rect(Rect{{200.0f, 50.0f, 400.0f, 150.0f}}, colors::u8::RED);
            cmd.add_line(Line{{50.0f, 200.0f}, {500.0f, 250.0f}}, colors::u8::CYAN);
        }

        const auto* gpu_device = a.world().try_get<RendererHandle>();
        const auto* window_handle = a.world().try_get<WindowHandle>();
        if (gpu_device == nullptr) {
            return;
        }
        if (window_handle == nullptr) {
            SDL_Log("Failed to get a window handle!");
            return;
        }
        SDL_GPUDevice* device = gpu_device->gpu_device;
        SDL_Window* window = window_handle->window;

        if (!create_ui_pipeline(device, window)) {
            return;
        }
        create_ui_buffers(a, device);
        upload_fullscreen_triangle(device);
    });

    app.add_systems(Schedule::Update, [](App& a) {
        const auto& world = a.world();
        const auto* gpu_device = world.try_get<RendererHandle>();
        const auto* window_handle = world.try_get<WindowHandle>();
        if (!gpu_device || !window_handle)
            return;
        SDL_GPUDevice* device = gpu_device->gpu_device;

        auto* cmdbuf = SDL_AcquireGPUCommandBuffer(device);
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

        auto& ui_buffers{a.world().get<UIRenderResources>()};
        auto& commands{a.world().get_mut<UICommandBuffer>().get_commands()};
        const u32 ui_count = std::min(commands.size(), MAX_NUMBER_UI_SHAPES);

        if (swapchain_texture != nullptr) {
            const auto time = SDL_GetTicks() / 1000.f;

            const auto g = static_cast<u8>(((std::sin(time / 2) + 1) / 2.0) * 255);
            const auto b = static_cast<u8>(((std::sin(time * 2) + 1) / 2.0) * 255);
            for (u32 i = 0; i < commands.size(); ++i) {
                auto& shape{commands[i]};
                const auto r = static_cast<u8>(((std::sin(time + i) + 1.0f) / 2.0) * 255);
                switch (shape.type) {
                case ShapeType::Circle:
                    shape.color = uColor32{r, g, b, 255};
                    shape.circle.radius = (std::sin(time * 2 + 29) + 1) / 2.0f * 10.0f + 10.0f;
                    shape.circle.center.x = (std::cos(time * i) + 1) / 2.0f * 200.0f + 100 * i;
                    shape.circle.center.y = (std::sin(time * i) + 1) / 2.0f * 200.0f + 20 * i;
                    break;
                case ShapeType::Line:
                    shape.line.a.x = shape.line.b.x + std::cos(time) * 10;
                    shape.line.a.y = shape.line.b.y + std::sin(time * 2) * 30;
                    break;
                case ShapeType::Rect:
                    shape.rect.rect.w = (std::sin(time) + 1) / 2.0 * 400;
                    shape.rect.rect.h = (std::cos(time * 3) + 1) / 2.0f * 150;
                    shape.color.a =
                        std::clamp<u8>((std::cos(time * 5) + 1) / 2.0f * 255u, 150, 255u);
                    break;
                }
            }

            {
                auto* ptr = static_cast<UICommand*>(
                    SDL_MapGPUTransferBuffer(device, ui_buffers.transfer_buffer, true));
                SDL_memcpy(ptr, commands.data(), ui_count * sizeof(UICommand));
                SDL_UnmapGPUTransferBuffer(device, ui_buffers.transfer_buffer);

                SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmdbuf);
                const SDL_GPUTransferBufferLocation src{
                    .transfer_buffer = ui_buffers.transfer_buffer,
                    .offset = 0,
                };
                const SDL_GPUBufferRegion dst{
                    .buffer = ui_buffers.storage_buffer,
                    .offset = 0,
                    .size = sizeof(UICommand) * ui_count,
                };
                SDL_UploadToGPUBuffer(copy_pass, &src, &dst, true);
                SDL_EndGPUCopyPass(copy_pass);
            }

            // Render SDFs
            {
                SDL_GPUColorTargetInfo color_target_info{
                    .texture = swapchain_texture,
                    .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
                    .load_op = SDL_GPU_LOADOP_CLEAR,
                    .store_op = SDL_GPU_STOREOP_STORE,
                };
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
                window_size = {.w = static_cast<f32>(w),
                               .h = static_cast<f32>(h),
                               .t = time,
                               .count = static_cast<u32>(commands.size()),
                               .color = hm::alloy::colors::f128::GRAY};
                SDL_PushGPUFragmentUniformData(cmdbuf, 0, &window_size, sizeof(window_size));

                SDL_GPUBuffer* storage_buffers[]{ui_buffers.storage_buffer};
                SDL_BindGPUFragmentStorageBuffers(render_pass, 0, storage_buffers, 1);

                const SDL_GPUBufferBinding binding{.buffer = vertex_buffer, .offset = 0};
                SDL_BindGPUVertexBuffers(render_pass, 0, &binding, 1);
                SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

                SDL_EndGPURenderPass(render_pass);
            }
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    });
}