#include "2d_renderer.hpp"
#include "prelude.hpp"

#include "entry.hpp"
#include "hm_2d_renderer.hpp"
#include "hm_window.hpp"

#include "window.hpp"
#include <cmath>

void hm_setup(hm::App& app) {
    app.add_plugin<hm::WindowPlugin>().add_plugin<hm::CompatibleRendererPlugin>();

    app.world().set(hm::WindowConfig{.title = "Clear Window", .width = 400, .height = 400});

    app.add_systems(hm::Schedule::Update, [](hm::App& a) {
        auto* render_handle = a.world().try_get<hm::RendererHandle>();
        auto* window_handle = a.world().try_get<hm::WindowHandle>();
        if (!render_handle || !render_handle->renderer || !window_handle->window)
            return;

        const auto t = SDL_GetTicks() / 1000.f;
        const auto r = (std::sin(t) + 1) / 2.0f;
        const auto g = (std::sin(t / 2) + 1) / 2.0f;
        const auto b = (std::sin(t * 2) + 1) / 2.0f;
        auto * renderer {render_handle->renderer};
        SDL_SetRenderDrawColorFloat(renderer,r,g,b,1.0f);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    });
}
