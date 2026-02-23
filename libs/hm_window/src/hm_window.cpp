#include "hm_window.hpp"
#include "app.hpp"
#include "window.hpp"

#include <SDL3/SDL.h>

namespace hm {

void WindowPlugin::build(App& app) {
    if (!app.get_resource<WindowConfig>())
        app.insert_resource(WindowConfig{});

    app.add_systems(Schedule::Startup, [](App& a) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("SDL_Init failed: %s", SDL_GetError());
            return;
        }

        auto* cfg = a.get_resource<WindowConfig>();
        auto* win =
            SDL_CreateWindow(cfg->title.c_str(), cfg->width, cfg->height, SDL_WINDOW_RESIZABLE);
        if (!win) {
            SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
            return;
        }

        auto* renderer = SDL_CreateRenderer(win, nullptr);
        if (!renderer) {
            SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
            SDL_DestroyWindow(win);
            return;
        }

        SDL_SetRenderVSync(renderer, 1);
        SDL_ShowWindow(win);

        a.insert_resource(WindowHandle{.window = win, .renderer = renderer});
        SDL_Log("Window created successfully");
    });

    app.add_systems(Schedule::Shutdown, [](App& a) {
        auto* h = a.get_resource_mut<WindowHandle>();
        if (!h)
            return;
        if (h->renderer)
            SDL_DestroyRenderer(h->renderer);
        if (h->window)
            SDL_DestroyWindow(h->window);
        h->renderer = nullptr;
        h->window = nullptr;
        SDL_Quit();
        SDL_Log("Window destroyed");
    });
}

} // namespace hm
