#include "hm_shader.hpp"

namespace hm::gpu {
static bool resolve_path(GPUDeviceHandle device, const char* filename, char* out, usize out_size,
                         SDL_GPUShaderFormat& format, const char*& entrypoint) {
    const SDL_GPUShaderFormat backends = SDL_GetGPUShaderFormats(device);
    const char* base = SDL_GetBasePath();
    if (backends & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_snprintf(out, out_size, "%sassets/shaders/compiled/spirv/%s.spv", base, filename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    } else if (backends & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_snprintf(out, out_size, "%sassets/shaders/compiled/msl/%s.msl", base, filename);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0";
    } else if (backends & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_snprintf(out, out_size, "%sassets/shaders/compiled/dxil/%s.dxil", base, filename);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } else {
        log::error("unrecognized GPU shader backend format");
        return false;
    }
    return true;
}
} // namespace hm::gpu
hm::ShaderHandle hm::gpu::load_shader(GPUDeviceHandle device, const char* filename,
                                      const ShaderResources& resources) {
    SDL_GPUShaderStage stage;
    if (SDL_strstr(filename, ".vert")) {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    } else if (SDL_strstr(filename, ".frag")) {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    } else {
        log::error("cannot infer shader stage from '{}'", filename);
        return nullptr;
    }

    char path[256];
    SDL_GPUShaderFormat format;
    const char* entrypoint;
    if (!resolve_path(device, filename, path, sizeof(path), format, entrypoint))
        return nullptr;

    usize code_size;
    void* code = SDL_LoadFile(path, &code_size);
    if (code == nullptr) {
        log::error("could not load shader '{}'", path);
        return nullptr;
    }

    const SDL_GPUShaderCreateInfo info{
        .code_size = code_size,
        .code = static_cast<const Uint8*>(code),
        .entrypoint = entrypoint,
        .format = format,
        .stage = stage,
        .num_samplers = resources.samplers,
        .num_storage_textures = resources.storage_textures,
        .num_storage_buffers = resources.storage_buffers,
        .num_uniform_buffers = resources.uniform_buffers,
    };
    ShaderHandle shader = check_sdl(SDL_CreateGPUShader(device, &info));
    SDL_free(code);
    return shader;
}

hm::ComputePipelineHandle hm::gpu::create_compute_pipeline(GPUDeviceHandle device,
                                                           const char* filename,
                                                           const ComputePipelineInfo& desc) {
    char path[256];
    SDL_GPUShaderFormat format;
    const char* entrypoint;
    if (!resolve_path(device, filename, path, sizeof(path), format, entrypoint))
        return nullptr;

    usize code_size;
    void* code = SDL_LoadFile(path, &code_size);
    if (code == nullptr) {
        log::error("could not load compute shader '{}'", path);
        return nullptr;
    }

    const SDL_GPUComputePipelineCreateInfo info{
        .code_size = code_size,
        .code = static_cast<const Uint8*>(code),
        .entrypoint = entrypoint,
        .format = format,
        .num_samplers = desc.samplers,
        .num_readonly_storage_textures = desc.readonly_storage_textures,
        .num_readonly_storage_buffers = desc.readonly_storage_buffers,
        .num_readwrite_storage_textures = desc.readwrite_storage_textures,
        .num_readwrite_storage_buffers = desc.readwrite_storage_buffers,
        .num_uniform_buffers = desc.uniform_buffers,
        .threadcount_x = desc.threadcount_x,
        .threadcount_y = desc.threadcount_y,
        .threadcount_z = desc.threadcount_z,
    };
    ComputePipelineHandle pipeline = check_sdl(SDL_CreateGPUComputePipeline(device, &info));
    SDL_free(code);
    return pipeline;
}