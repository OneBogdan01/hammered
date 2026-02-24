#include "hm_window.hpp"
#include "app.hpp"
#include "window.hpp"

#include <SDL3/SDL.h>

namespace hm {

WindowModule::WindowModule(World& world) {
    world.component<WindowConfig>();

    if (world.has<WindowConfig>() == false) {
        world.add<WindowConfig>();
    }
    WindowConfig cfg{world.get<WindowConfig>()};

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        flecs::log::err("SDL_Init failed: {}", SDL_GetError());

        return;
    }

    auto* win = SDL_CreateWindow(cfg.title.c_str(), cfg.width, cfg.height, SDL_WINDOW_RESIZABLE);
    if (!win) {
        flecs::log::err("SDL_CreateWindow failed: {}", SDL_GetError());
        return;
    }

    // TODO renderer is another story
    auto* renderer = SDL_CreateRenderer(win, nullptr);
    if (!renderer) {
        flecs::log::err("SDL_CreateRenderer failed: {}", SDL_GetError());
        SDL_DestroyWindow(win);
        return;
    }

    SDL_SetRenderVSync(renderer, 1);

    world.set<WindowHandle>({.window = win, .renderer = renderer});

    flecs::log::trace("Window created miraculously");

    world.atfini() app.add_systems(Schedule::Shutdown, [](App& a) {
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
