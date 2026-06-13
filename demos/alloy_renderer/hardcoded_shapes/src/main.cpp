#include "prelude.hpp"

#include "hm_alloy.hpp"
#include "hm_gpu.hpp"
#include "renderer.hpp"

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

    SDL_GPUShader* fragment_shader = hm::alloy::LoadShader(device, "ui_canvas.frag", 0, 1, 0, 0);
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
                .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = SDL_GPU_BLENDOP_ADD,
                .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
                .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                .color_write_mask = 0xF,
                .enable_blend = true,
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

static void upload_fullscreen_triangle(SDL_GPUDevice* device) {
    SDL_GPUBufferCreateInfo vbuf_info{.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                      .size = sizeof(Vertex) * 3};
    vertex_buffer = SDL_CreateGPUBuffer(device, &vbuf_info);

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
hm::GPUDeviceHandle g_device;
hm::WindowHandle g_window;
void hm_setup(hm::App& app) {
    using namespace hm;

    app.add_plugin<WindowPlugin>(
           WindowConfig{.title = "Rectangle UI", .width = 640, .height = 320})
        .add_plugin<GPUPlugin>();

    app.add_systems(Schedule::Startup, [](App& a) {
        auto gpu_plugin = a.get_plugin_mutable<GPUPlugin>();
        auto window_plugin = a.get_plugin_mutable<WindowPlugin>();
        if (!gpu_plugin || !window_plugin) {
            log::error("hardcoded_shapes needs WindowPlugin and GPUPlugin");
            return;
        }
         g_device = gpu_plugin->get().get_gpu_handle();
         g_window = window_plugin->get().get_window_handle();
        if (g_device == nullptr || g_window == nullptr) {
            log::error("GPU device or window was not initialized");
            return;
        }

        if (!create_ui_pipeline(g_device, g_window)) return;
        upload_fullscreen_triangle(g_device);
    });

    app.add_systems(Schedule::Update, [](App&) {
        if (g_device == nullptr || g_window == nullptr) return;

        auto* cmdbuf = check_sdl(SDL_AcquireGPUCommandBuffer(g_device));
        if (cmdbuf == nullptr) return;

        SDL_GPUTexture* swapchain_texture;
        if (!check_sdl(SDL_WaitAndAcquireGPUSwapchainTexture(
                cmdbuf, g_window, &swapchain_texture, nullptr, nullptr))) {
            return;
        }

        if (swapchain_texture != nullptr) {
            const auto time = SDL_GetTicks() / 1000.f;

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
                f32 w, h, t;
            } window_size;
            int w, h;
            SDL_GetWindowSizeInPixels(g_window, &w, &h);
            window_size = {.w = static_cast<f32>(w), .h = static_cast<f32>(h), .t = time};
            SDL_PushGPUFragmentUniformData(cmdbuf, 0, &window_size, sizeof(window_size));

            const SDL_GPUBufferBinding binding{.buffer = vertex_buffer, .offset = 0};
            SDL_BindGPUVertexBuffers(render_pass, 0, &binding, 1);
            SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

            SDL_EndGPURenderPass(render_pass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    });
}