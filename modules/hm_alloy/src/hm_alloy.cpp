#include "hm_alloy.hpp"
#include "app.hpp"
#include "hm_sdl.hpp"
#include "hm_window.hpp"

namespace hm::alloy {

void AlloyPlugin::build(App& app) {
    app.add_systems(Schedule::Startup, [this](App& a) {

        m_command_buffer = CommandBuffer{};
    });

    app.add_systems(Schedule::Shutdown, [this](App& a) {

    });
}

} // namespace hm::alloy