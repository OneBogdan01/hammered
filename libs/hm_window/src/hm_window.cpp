#include "hm_window.hpp"

#include "app.hpp"
#include "window.hpp"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
namespace {
SDL_Window* window = nullptr;
}

using namespace hm;
void WindowPlugin::build(App& app) {
    // TODO add events for windowing
    // app.add_message::<WindowEvent>()
    //     .add_message::<WindowResized>()
    //     .add_message::<WindowCreated>()
    //     .add_message::<WindowClosing>()
    //     .add_message::<WindowClosed>()
    //     .add_message::<WindowCloseRequested>()
    //     .add_message::<WindowDestroyed>()
    //     .add_message::<RequestRedraw>()
    //     .add_message::<CursorMoved>()
    //     .add_message::<CursorEntered>()
    //     .add_message::<CursorLeft>()
    //     .add_message::<Ime>()
    //     .add_message::<WindowFocused>()
    //     .add_message::<WindowOccluded>()
    //     .add_message::<WindowScaleFactorChanged>()
    //     .add_message::<WindowBackendScaleFactorChanged>()
    //     .add_message::<FileDragAndDrop>()
    //     .add_message::<WindowMoved>()
    //     .add_message::<WindowThemeChanged>()
    //     .add_message::<AppLifecycle>();
    app.insert_resource(Window{.title = "My Game", .width = 640, .height = 480});
    app.add_systems(Schedule::Startup, create_window);
    app.add_systems(Schedule::First, poll_events);
    app.add_systems(Schedule::PostShutdown, destroy_window);
}
void create_window(Commands& cmd, Res<WindowConfig> config) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        // panic
    }

    window = SDL_CreateWindow(config->title.c_str(), config->width, config->height,
                              SDL_WINDOW_RESIZABLE);

    if (!window) {
        // panic
    }
}
