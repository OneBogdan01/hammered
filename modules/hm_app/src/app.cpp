#include "app.hpp"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <print>
#include <hm/version.hpp>

hm::App::App() {
    std::println("hm engine v.{}.{}.{}", version_major, version_minor, version_patch);
}
void hm::App::startup() const {
    for (auto& fn : m_startup_fn)
        fn();
}
void hm::App::update() const {}
void hm::App::shutdown() const {
    for (auto& fn : m_shutdown_fn | std::views::reverse) {
        fn();
    }
}

/// SDL Callbacks
SDL_AppResult SDL_AppInit(void** appstate, int, char**) {
    auto* app = new hm::App();
    app->startup();
    *appstate = app;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    // TODO Input and other OS specific events
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = static_cast<hm::App*>(appstate);
    // update everything in the loop
    app->update();
    return SDL_APP_CONTINUE;
}
void SDL_AppQuit(void* appstate, SDL_AppResult) {
    auto* app = static_cast<hm::App*>(appstate);
    if (app) {
        app->shutdown();
        delete app;
    }
}
