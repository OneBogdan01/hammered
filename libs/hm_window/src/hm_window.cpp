#include "hm_window.hpp"
#include "app.hpp"
#include "window.hpp"

#include <SDL3/SDL.h>

namespace hm {

WindowModule::WindowModule(World& world) {
    world.module<WindowModule>();

    world.component<WindowConfig>();

    if (world.has<WindowConfig>() == false) {
        world.add<WindowConfig>();
    }
    WindowConfig cfg{world.get<WindowConfig>()};

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: {}", SDL_GetError());

        return;
    }

    auto* win = SDL_CreateWindow(cfg.title.c_str(), cfg.width, cfg.height, SDL_WINDOW_RESIZABLE);
    if (!win) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                        ("SDL_CreateWindow failed: {}", SDL_GetError()));
        return;
    }

    // TODO renderer is another story
    auto* renderer = SDL_CreateRenderer(win, nullptr);
    if (!renderer) {
        SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, ("SDL_CreateRenderer failed: {}", SDL_GetError()));
        SDL_DestroyWindow(win);
        return;
    }

    SDL_SetRenderVSync(renderer, 1);

    world.set<WindowHandle>({.window = win, .renderer = renderer});

    /* world.atfini([](World& world) {
         auto* h = world->try_get<WindowHandle>();
         if (!h)
             return;

         if (h->renderer)
             SDL_DestroyRenderer(h->renderer);
         if (h->window)
             SDL_DestroyWindow(h->window);

         SDL_Quit();
         SDL_Log("Window destroyed");
     });*/
}

} // namespace hm
