#pragma once
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

namespace hm {
class App;
}
// User implements this per demo/app
extern void hm_setup(hm::App& app);

// SDL callbacks must be at global scope with C linkage —
// SDL_main.h handles the extern "C" for you, but they
// cannot be inside a namespace.

inline SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    auto* app = new hm::App();
    hm_setup(*app);
    app->startup();
    *appstate = app;
    return SDL_APP_CONTINUE;
}

inline SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    // TODO: forward events once hm::Events<T> is implemented
    // auto* app = static_cast<hm::App*>(appstate);
    // app->world().resource_mut<hm::Events<SDL_Event>>().send(*event);

    return SDL_APP_CONTINUE;
}

inline SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = static_cast<hm::App*>(appstate);
    app->update();
    return SDL_APP_CONTINUE;
}

inline void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = static_cast<hm::App*>(appstate);
    if (app) {
        app->shutdown();
        delete app;
    }
}
