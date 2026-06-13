#pragma once
#include "plugin.hpp"
#include "hm_sdl.hpp"

namespace hm {

struct WindowConfig {
    std::string title = "Hammered Engine";
    i32 width{800};
    i32 height{600};
    bool b_vsync{true};
    WindowFlags window_flags{window_flags::RESIZABLE};
};
class WindowPlugin : public Plugin {
  public:
    WindowPlugin(WindowConfig cfg = {});
    ~WindowPlugin() override = default;
    HM_PLUGIN(Window)
    void build(App& app) override;

    WindowHandle get_window_handle() const {
        return m_window_handle;
    };


  private:
    WindowConfig m_cfg{};
    WindowHandle m_window_handle{nullptr};
};
} // namespace hm
