#include "command_buffer_ui.hpp"
#include "prelude.hpp"

#include "entry.hpp"
#include "hm_alloy_ui.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <algorithm>
#include <print>

static SDL_GPUGraphicsPipeline* pipeline;
static SDL_GPUBuffer* vertex_buffer;
static SDL_GPUComputePipeline* compute_pipeline;
static SDL_GPUBuffer* tile_buffer_count;
static SDL_GPUBuffer* tile_buffer_indicies;
static SDL_GPUTransferBuffer* tile_count_clear;
constexpr hm::u32 MAX_ENTRIES_PER_TILE{200};
static hm::u32 tiles_x;
static hm::u32 tiles_y;
static hm::u32 tile_count;
constexpr hm::u32 TILE_SIZE{8};
struct Vertex {
    float x, y, z;
};

// Tile buffer
// TODO this is super slow right now and very wasteful with memory
struct TilePrimitiveCounts {
    hm::u32 count;
};
// entries per tile
struct TilePrimitiveIndex {
    hm::u32 index;
};
void hm_setup(hm::App& app) {
    using namespace hm;
    using namespace hm::alloy;

    app.world().set<WindowConfig>({.title = "Alloy Buffered Shapes", .width = 640, .height = 320});
    app.add_plugin<WindowPlugin>().add_plugin<AlloyUiPlugin>();
    std::println("Please do not resize, this does not handle that at all right now!");
    std::println("Press space to see the debug tiled heatmap!");

    app.add_systems(Schedule::Startup, [](App& a) {
        auto& cmd{a.world().get_mut<UICommandBuffer>()};

        // Some shapes being added to the buffer
        {
            cmd.add_circle(Circle{{300.0f, 50.0f}, 50.0f}, colors::u8::WHITE);
            cmd.add_rect(Rect{{200.0f, 50.0f, 400.0f, 150.0f}}, colors::u8::RED);
            cmd.add_line(Line{{50.0f, 200.0f}, {500.0f, 250.0f}}, colors::u8::CYAN);
            cmd.add_circle(Circle{{300.0f, 50.0f}, 50.0f}, colors::u8::WHITE);

            constexpr u32 COLS = 1000;
            constexpr u32 N = 10'000;
            for (u32 i = 4; i < N; ++i) {
                float x = static_cast<float>(i % COLS) / COLS * 640.0f;
                float y = static_cast<float>(i / COLS) / (N / COLS) * 320.0f;
                cmd.add_circle(Circle{{x, y}, 6.0f}, colors::u8::WHITE);
            }
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

        // make the pipeline for the ui
        {
            // Create the shaders
            SDL_GPUShader* vertexShader =
                LoadShader(gpu_device->gpu_device, "ui_canvas.vert", 0, 0, 0, 0);
            if (vertexShader == NULL) {
                SDL_Log("Failed to create vertex shader!");
                return;
            }

            SDL_GPUShader* fragmentShader =
                LoadShader(gpu_device->gpu_device, "ui_canvas.frag", 0, 1, 3, 0);
            if (fragmentShader == NULL) {
                SDL_Log("Failed to create fragment shader!");
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
        }
        // compute setup for tiled rendering
        {
            // this will need to be called whenever a resize happens
            i32 w, h;
            SDL_GetWindowSizeInPixels(window_handle->window, &w, &h);

            // Round properly for weird resolutions that do not fit perfectly with the tile size.
            tiles_x = (w + TILE_SIZE - 1) / TILE_SIZE;
            tiles_y = (h + TILE_SIZE - 1) / TILE_SIZE;
            tile_count = tiles_y * tiles_x;
            std::println("There are {} tiles, for resolution {} x {}", tile_count, w, h);
            SDL_GPUComputePipelineCreateInfo info{.num_readonly_storage_buffers = 1,
                                                  .num_readwrite_storage_buffers = 2,
                                                  .num_uniform_buffers = 1,
                                                  .threadcount_x = 64,
                                                  .threadcount_y = 1,
                                                  .threadcount_z = 1};
            // Create the sprite batch compute pipeline
            compute_pipeline =
                CreateComputePipelineFromShader(gpu_device->gpu_device, "tile_shapes.comp", &info);
            SDL_GPUBufferCreateInfo buffer_create_info{
                .usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE |
                         SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
                .size = static_cast<u32>(sizeof(TilePrimitiveIndex)) * tile_count *
                        MAX_ENTRIES_PER_TILE};
            tile_buffer_indicies = SDL_CreateGPUBuffer(gpu_device->gpu_device, &buffer_create_info);

            SDL_GPUBufferCreateInfo buffer_count_create_info{
                .usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE |
                         SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
                .size = static_cast<u32>(sizeof(TilePrimitiveCounts)) * tile_count};
            tile_buffer_count =
                SDL_CreateGPUBuffer(gpu_device->gpu_device, &buffer_count_create_info);
            // SDL GPU has no "clear buffer" command, so we reset counts by copying zeros.
            SDL_GPUTransferBufferCreateInfo clear_info{
                .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                .size = static_cast<u32>(sizeof(TilePrimitiveCounts)) * tile_count};
            tile_count_clear = SDL_CreateGPUTransferBuffer(gpu_device->gpu_device, &clear_info);
            u32* zeros = static_cast<u32*>(
                SDL_MapGPUTransferBuffer(gpu_device->gpu_device, tile_count_clear, false));
            SDL_memset(zeros, 0, sizeof(u32) * tile_count);
            SDL_UnmapGPUTransferBuffer(gpu_device->gpu_device, tile_count_clear);
        }
        {
            // Create the vertex buffer for the triangle that covers the whole screen
            SDL_GPUBufferCreateInfo buffer_create_info{.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                                       .size = sizeof(Vertex) * 3};
            vertex_buffer = SDL_CreateGPUBuffer(gpu_device->gpu_device, &buffer_create_info);
            // prepare the buffer for UI

            auto& ui_buffers{a.world().ensure<UIRenderResources>()};
            SDL_GPUTransferBufferCreateInfo ui_transfer_info{
                .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                .size = MAX_NUMBER_UI_SHAPES / 4 * sizeof(UICommand)};
            ui_buffers.transfer_buffer =
                SDL_CreateGPUTransferBuffer(gpu_device->gpu_device, &ui_transfer_info);
            SDL_GPUBufferCreateInfo ui_buffer_info{
                .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ |
                         SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ,
                .size = MAX_NUMBER_UI_SHAPES / 4 * sizeof(UICommand)};
            ui_buffers.storage_buffer =
                SDL_CreateGPUBuffer(gpu_device->gpu_device, &ui_buffer_info);
        }
        // Get Triangle to cover the whole screen from the vertex side
        {
            SDL_GPUTransferBufferCreateInfo buffer_info{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                                        .size = sizeof(Vertex) * 3};
            auto* transferBuffer =
                SDL_CreateGPUTransferBuffer(gpu_device->gpu_device, &buffer_info);

            Vertex* transferData = static_cast<Vertex*>(
                SDL_MapGPUTransferBuffer(gpu_device->gpu_device, transferBuffer, false));

            transferData[0] = Vertex{-1, 1, 0};
            transferData[1] = Vertex{-1, -3, 0};
            transferData[2] = Vertex{3, 1, 0};

            SDL_UnmapGPUTransferBuffer(gpu_device->gpu_device, transferBuffer);

            // Upload the transfer data to the vertex buffer
            SDL_GPUCommandBuffer* uploadCmdBuf =
                SDL_AcquireGPUCommandBuffer(gpu_device->gpu_device);
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

            SDL_GPUTransferBufferLocation buffer_location{.transfer_buffer = transferBuffer,
                                                          .offset = 0};
            SDL_GPUBufferRegion buffer_region{
                .buffer = vertex_buffer, .offset = 0, .size = sizeof(Vertex) * 3};
            SDL_UploadToGPUBuffer(copyPass, &buffer_location, &buffer_region, false);

            SDL_EndGPUCopyPass(copyPass);
            SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
            SDL_ReleaseGPUTransferBuffer(gpu_device->gpu_device, transferBuffer);
        }
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
        // ui update!
        auto& ui_buffers{a.world().get<UIRenderResources>()};
        auto& commands{a.world().get_mut<UICommandBuffer>().get_commands()};
        const u32 ui_count = std::min(commands.size(), MAX_NUMBER_UI_SHAPES);

        if (swapchain_texture != nullptr) {
            const auto time = SDL_GetTicks() / 1000.f;

            static u32 mode = 0; // 0 brute, 1 tiled, 2 heatmap
            static bool space_was_down = false;
            const bool* keys = SDL_GetKeyboardState(nullptr);
            const bool space_down = keys[SDL_SCANCODE_SPACE];

            // --- perf timing (performance counter) ---
            static const Uint64 perf_freq = SDL_GetPerformanceFrequency();
            static Uint64 last_tick = SDL_GetPerformanceCounter();
            static double accum_ms = 0.0;
            static u32 frames = 0;

            if (space_down && !space_was_down) {
                mode = (mode + 1u) % 3u;
                const char* names[]{"brute force", "tiled", "heatmap"};
                std::println("--- mode -> {} ---", names[mode]);
                accum_ms = 0.0;
                frames = 0;                              // discard old mode's samples
                last_tick = SDL_GetPerformanceCounter(); // <-- start the clock fresh
            }
            space_was_down = space_down;

            const Uint64 now = SDL_GetPerformanceCounter();
            accum_ms += static_cast<double>(now - last_tick) / static_cast<double>(perf_freq) * 1000.0;
            last_tick = now;
            if (++frames >= 60) {
                const char* names[]{"brute force", "tiled", "heatmap"};
                std::println("{} | {} shapes | {:.4f} ms/frame ({:.0f} fps)", names[mode], ui_count,
                             accum_ms / frames, frames * 1000.0 / accum_ms);
                accum_ms = 0.0;
                frames = 0;
            }

            for (auto i = 0; i < commands.size(); i++) {
                auto& shape{commands[i]};
                const auto r = static_cast<u8>(((std::sin(time + i) + 1.0f) / 2.0) * 255);
                const auto g = static_cast<u8>(((std::sin(time / 2) + 1) / 2.0) * 255);
                const auto b = static_cast<u8>(((std::sin(time * 2) + 1) / 2.0) * 255);
                switch (shape.type) {
                case ShapeType::Circle:
                    shape.color = uColor32{r, g, b, 255};
                    shape.circle.radius = (std::sin(time * 2 + i) + 1) / 2.0f * 4.0f + 4.0f;
                    // wrap position to the screen instead of running off it
                    shape.circle.center.x =
                        std::fmod(std::sin(time + i * 0.3f) * 320.0f + 320.0f, 640.0f);
                    shape.circle.center.y =
                        std::fmod(std::cos(time + i * 0.7f) * 160.0f + 160.0f, 320.0f);
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
                // Build sprite instance transfer
                auto* ptr = static_cast<UICommand*>(SDL_MapGPUTransferBuffer(
                    gpu_device->gpu_device, ui_buffers.transfer_buffer, true));

                SDL_memcpy(ptr, commands.data(), ui_count * sizeof(UICommand));

                SDL_UnmapGPUTransferBuffer(gpu_device->gpu_device, ui_buffers.transfer_buffer);

                // Upload data
                SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdbuf);
                const SDL_GPUTransferBufferLocation src{
                    .transfer_buffer = ui_buffers.transfer_buffer,
                    .offset = 0,
                };

                const SDL_GPUBufferRegion dst{
                    .buffer = ui_buffers.storage_buffer,
                    .offset = 0,
                    .size = sizeof(UICommand) * ui_count,
                };

                SDL_UploadToGPUBuffer(copyPass, &src, &dst, true);

                const SDL_GPUTransferBufferLocation clear_src{.transfer_buffer = tile_count_clear,
                                                              .offset = 0};
                const SDL_GPUBufferRegion clear_dst{.buffer = tile_buffer_count,
                                                    .offset = 0,
                                                    .size =
                                                        static_cast<u32>(sizeof(u32)) * tile_count};
                SDL_UploadToGPUBuffer(copyPass, &clear_src, &clear_dst, false);

                SDL_EndGPUCopyPass(copyPass);
            }

            {
                {
                    SDL_GPUStorageBufferReadWriteBinding rw[]{
                        {.buffer = tile_buffer_count, .cycle = false},
                        {.buffer = tile_buffer_indicies, .cycle = false},
                    };
                    SDL_GPUComputePass* compute_pass =
                        SDL_BeginGPUComputePass(cmdbuf, nullptr, 0, rw, 2);

                    SDL_BindGPUComputePipeline(compute_pass, compute_pipeline);
                    SDL_BindGPUComputeStorageBuffers(compute_pass, 0, &ui_buffers.storage_buffer,
                                                     1);

                    struct TileParams {
                        u32 tiles_x;
                        u32 tiles_y;
                        u32 tile_size;
                        u32 count;
                    } params{tiles_x, tiles_y, TILE_SIZE, ui_count};
                    SDL_PushGPUComputeUniformData(cmdbuf, 0, &params, sizeof(params));

                    SDL_DispatchGPUCompute(compute_pass, (ui_count + 63) / 64, 1, 1);
                    SDL_EndGPUComputePass(compute_pass);
                }
            }
            // render sdf shapes
            {
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
                    u32 tiles_x;
                    u32 tile_size;
                    u32 mode = 0;
                } window_size;
                int w, h;
                SDL_GetWindowSizeInPixels(window_handle->window, &w, &h);
                window_size = {.w = static_cast<f32>(w),
                               .h = static_cast<f32>(h),
                               .t = time,
                               .count = static_cast<u32>(commands.size()),
                               .color = hm::alloy::colors::f128::GRAY,
                               .tiles_x = tiles_x,
                               .tile_size = TILE_SIZE,
                               .mode = mode};
                SDL_PushGPUFragmentUniformData(cmdbuf, 0, &window_size, sizeof(window_size));

                SDL_GPUBuffer* storage_buffers[]{ui_buffers.storage_buffer, tile_buffer_count,
                                                 tile_buffer_indicies};
                SDL_BindGPUFragmentStorageBuffers(render_pass, 0, storage_buffers, 3);

                const SDL_GPUBufferBinding binding{.buffer = vertex_buffer, .offset = 0};
                SDL_BindGPUVertexBuffers(render_pass, 0, &binding, 1);
                SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

                SDL_EndGPURenderPass(render_pass);
            }
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    });
}