#include "hm_window.hpp"
#include "app.hpp"
#include "window.hpp"

#include <SDL3/SDL.h>

namespace hm {
void WindowPlugin::build(App& app) {
    auto& world = app.world();
    world.ensure<WindowConfig>();

    app.add_systems(Schedule::Startup, [](App& a) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
            return;
        }

        const auto* cfg = a.world().try_get<WindowConfig>();
        assert(cfg);

        auto* win =
            SDL_CreateWindow(cfg->title.c_str(), cfg->width, cfg->height, SDL_WINDOW_RESIZABLE);
        if (!win) {
            SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
            return;
        }

        a.world().set<WindowHandle>({.window = win});
        SDL_Log("Window created successfully");
    });

    app.add_systems(Schedule::Shutdown, [](App& a) {
        auto& world = a.world();

        auto& handle = world.ensure<WindowHandle>();
        if (handle.window)
            SDL_DestroyWindow(handle.window);

        handle.window = nullptr;
        SDL_Quit();
        SDL_Log("Window destroyed");
    });
}
} // namespace hm