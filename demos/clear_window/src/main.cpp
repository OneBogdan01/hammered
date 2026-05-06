#include "prelude.hpp"

#include "entry.hpp"
#include "hm_window.hpp"
#include "window.hpp"
#include <cmath>
void hm_setup(hm::App& app) {
    // app.insert_resource(hm::WindowConfig{.title = "Hello Hammered", .width = 400, .height = 400});
    // app.add_plugin<hm::WindowPlugin>();

    // app.add_systems(hm::Schedule::Update, [](hm::App& a) {
    //     auto* h = a.get_resource<hm::WindowHandle>();
    //     if (!h || !h->renderer)
    //         return;
    //
    //     auto t = SDL_GetTicks() / 1000.f;
    //     auto r = static_cast<uint8_t>((std::sin(t) + 1) / 2.0 * 255);
    //     auto g = static_cast<uint8_t>((std::sin(t / 2) + 1) / 2.0 * 255);
    //     auto b = static_cast<uint8_t>((std::sin(t * 2) + 1) / 2.0 * 255);
    //
    //     SDL_SetRenderDrawColor(h->renderer, r, g, b, SDL_ALPHA_OPAQUE);
    //     SDL_RenderClear(h->renderer);
    //     SDL_RenderPresent(h->renderer);
    // });
}
