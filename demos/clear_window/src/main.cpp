#include "prelude.hpp"

#include "entry.hpp"
#include "hm_window.hpp"
#include "window.hpp"
#include <cmath>
#include <print>
void hm_setup(hm::World& world) {
    world.set(hm::WindowConfig{.title = "Hello Hammered", .width = 400, .height = 400});
    world.import <hm::WindowModule>();

    world.system<hm::WindowHandle>("Update Color")
        .kind(flecs::OnUpdate)
        .each([&world](flecs::iter& it, size_t index, hm::WindowHandle& h) {
            if (!h.renderer)
                return;

            auto t = static_cast<float>(world.get_info()->world_time_total);
            auto r = static_cast<uint8_t>((std::sin(t) + 1) / 2.0 * 255);
            auto g = static_cast<uint8_t>((std::sin(t / 2) + 1) / 2.0 * 255);
            auto b = static_cast<uint8_t>((std::sin(t * 2) + 1) / 2.0 * 255);

            std::println("Delta time is {}", world.delta_time());
            // TODO move to renderer
            SDL_SetRenderDrawColor(h.renderer, r, g, b, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(h.renderer);
            SDL_RenderPresent(h.renderer);
        });
}
