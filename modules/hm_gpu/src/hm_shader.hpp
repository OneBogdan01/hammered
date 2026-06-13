#pragma once
#include "pch.hpp"
#include "hm_sdl.hpp"
namespace hm::gpu {
struct ShaderResources {
    u32 samplers{0};
    u32 uniform_buffers{0};
    u32 storage_buffers{0};
    u32 storage_textures{0};
};

[[nodiscard]] ShaderHandle load_shader(GPUDeviceHandle device, const char* filename,
                                       const ShaderResources& resources = {});

struct ComputePipelineInfo {
    u32 samplers{0};
    u32 readonly_storage_textures{0};
    u32 readonly_storage_buffers{0};
    u32 readwrite_storage_textures{0};
    u32 readwrite_storage_buffers{0};
    u32 uniform_buffers{0};
    u32 threadcount_x{1};
    u32 threadcount_y{1};
    u32 threadcount_z{1};
};

[[nodiscard]] ComputePipelineHandle
create_compute_pipeline(GPUDeviceHandle device, const char* filename,
                        const ComputePipelineInfo& desc = {});

} // namespace hm::gpu