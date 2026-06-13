#pragma once
#include "hm_sdl.hpp"
#include "pch.hpp"
namespace hm::gpu {

[[nodiscard]] BufferHandle create_buffer(GPUDeviceHandle device, SDL_GPUBufferUsageFlags usage,
                                         const u32 size) {
    const SDL_GPUBufferCreateInfo info{.usage = usage, .size = size};
    return check_sdl(SDL_CreateGPUBuffer(device, &info));
}

void upload_buffer(GPUDeviceHandle device, BufferHandle dst, const void* data, u32 size) {
    const SDL_GPUTransferBufferCreateInfo tinfo{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                                .size = size};
    auto* transfer = check_sdl(SDL_CreateGPUTransferBuffer(device, &tinfo));
    if (transfer == nullptr)
        return;

    void* mapped = SDL_MapGPUTransferBuffer(device, transfer, false);
    SDL_memcpy(mapped, data, size);
    SDL_UnmapGPUTransferBuffer(device, transfer);

    auto* cmd = SDL_AcquireGPUCommandBuffer(device);
    auto* copy = SDL_BeginGPUCopyPass(cmd);
    const SDL_GPUTransferBufferLocation src{.transfer_buffer = transfer, .offset = 0};
    const SDL_GPUBufferRegion dstr{.buffer = dst, .offset = 0, .size = size};
    SDL_UploadToGPUBuffer(copy, &src, &dstr, false);
    SDL_EndGPUCopyPass(copy);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(device, transfer);
}
} // namespace hm::gpu