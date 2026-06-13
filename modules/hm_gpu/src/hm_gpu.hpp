#pragma once
#include "plugin.hpp"
#include "hm_sdl.hpp"

namespace hm {

struct GPUConfig {
    PresentMode present_mode{present_mode::VSYNC};
    SwapchainComposition composition{swapchain_composition::SDR};
};
class GPUPlugin : public Plugin {
  public:
    GPUPlugin(GPUConfig cfg = {});
    ~GPUPlugin() override = default;
    HM_PLUGIN(Window)
    void build(App& app) override;

    GPUDeviceHandle get_gpu_handle() const {
        return m_gpu_device;
    };


  private:
    GPUConfig m_cfg{};
    GPUDeviceHandle m_gpu_device{nullptr};

};
} // namespace hm
