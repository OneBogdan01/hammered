#include "hm_window.hpp"
#include "app.hpp"
#include "hm_sdl.hpp"


namespace hm {
WindowPlugin::WindowPlugin(const WindowConfig cfg) : m_cfg(cfg) {}
void WindowPlugin::build(App& app) {
    app.add_systems(Schedule::Startup, [this](App&) {
        // TODO add proper flags here based on build
        if (!check_sdl(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)))
            return;

        m_window_handle = check_sdl(
            SDL_CreateWindow(m_cfg.title.c_str(), m_cfg.width, m_cfg.height, m_cfg.window_flags));
        if (!m_window_handle)
            return;

        log::info("Window created successfully");
    });

    app.add_systems(Schedule::Shutdown, [this](App&) {
        if (m_window_handle)
            SDL_DestroyWindow(m_window_handle);
        m_window_handle = nullptr;
        SDL_Quit();
        log::info("Window destroyed");
    });
}
} // namespace hm