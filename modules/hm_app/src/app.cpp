#include "app.hpp"

#include <print>
#include <hm/version.hpp>

hm::App::App() {
    std::println("hm engine v.{}.{}.{}", version_major, version_minor, version_patch);
}
void hm::App::startup() const {
    for (auto& fn : m_startup_fn)
        fn();
}
void hm::App::update() const {
    m_world.progress();
}
void hm::App::shutdown() {
    while (!m_shutdown_fn.empty()) {
        m_shutdown_fn.top()();
        m_shutdown_fn.pop();
    }
    m_world.quit();
}
