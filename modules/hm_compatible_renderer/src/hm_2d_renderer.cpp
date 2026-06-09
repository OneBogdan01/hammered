#include "hm_2d_renderer.hpp"
#include "app.hpp"
#include "2d_renderer.hpp"
#include "window.hpp"
#include "SDL3/SDL_log.h"
namespace hm {
void CompatibleRendererPlugin::build(App& app) {

    app.add_systems(Schedule::Startup, [](App& a) {
        auto& world = a.world();
            world.ensure<RendererHandle>();
            auto* window{world.get<WindowHandle>().window};

            auto* renderer = SDL_CreateRenderer(window, nullptr);
            if (!renderer) {
                SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
                SDL_DestroyWindow(window);
                return;
            }
            world.set<RendererHandle>({renderer});
    });

    app.add_systems(Schedule::Shutdown, [](App& a) {

    });
}
} // namespace hm