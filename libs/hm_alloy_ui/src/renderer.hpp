#pragma once

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_filesystem.h"
namespace hm {
struct RendererHandle {
    SDL_GPUDevice* gpu_device{nullptr};

};
struct PositionColorVertex {
    float x, y, z;
    Uint8 r, g, b, a;
};
inline SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const char* shaderFilename, Uint32 samplerCount,
                          Uint32 uniformBufferCount, Uint32 storageBufferCount,
                          Uint32 storageTextureCount) {
    // Auto-detect the shader stage from the file name for convenience
    SDL_GPUShaderStage stage;
    if (SDL_strstr(shaderFilename, ".vert")) {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    } else if (SDL_strstr(shaderFilename, ".frag")) {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    } else {
        SDL_Log("Invalid shader stage!");
        return nullptr;
    }

    char fullPath[256];
    SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
    const char* entrypoint;
    auto BasePath = SDL_GetBasePath();
    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sassets/shaders/compiled/spirv/%s.spv", BasePath,
                     shaderFilename);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entrypoint = "main";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sassets/shaders/compiled/msl/%s.msl", BasePath,
                     shaderFilename);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entrypoint = "main0";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_snprintf(fullPath, sizeof(fullPath), "%sassets/shaders/compiled/dxil/%s.dxil", BasePath,
                     shaderFilename);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entrypoint = "main";
    } else {
        SDL_Log("%s", "Unrecognized backend shader format!");
        return nullptr;
    }

    size_t codeSize;
    void* code = SDL_LoadFile(fullPath, &codeSize);
    if (code == nullptr) {
        SDL_Log("Failed to load shader from disk! %s", fullPath);
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shaderInfo;
    shaderInfo = {.code_size = codeSize,
                  .code = static_cast<const Uint8*>(code),
                  .entrypoint = entrypoint,
                  .format = format,
                  .stage = stage,
                  .num_samplers = samplerCount,
                  .num_storage_textures = storageTextureCount,
                  .num_storage_buffers = storageBufferCount,
                  .num_uniform_buffers = uniformBufferCount};
    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
    if (shader == nullptr) {
        SDL_Log("Failed to create shader!");
        SDL_free(code);
        return nullptr;
    }

    SDL_free(code);
    return shader;
}

} // namespace hm