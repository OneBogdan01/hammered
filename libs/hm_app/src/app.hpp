#pragma once
#include "plugin.hpp"

#include <entt/entt.hpp>
#include <memory>

#include <vector>

namespace hm {
class App {
  public:
    static App create() {
        return {};
    }

    template <typename P, typename... Args>
    App& add_plugin(Args&&... args) {
        m_plugins.push_back(std::make_unique<P>(std::forward<Args>(args)...));
        m_plugins.back()->build(*this);
        return *this;
    }

    template <typename T, typename... Args>
    T& emplace_resource(Args&&... args) {
        return m_registry.ctx().emplace<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    T& insert_resource(T&& resource) {
        return m_registry.ctx().emplace<T>(std::forward<T>(resource));
    }

    template <typename T>
    T* get_resource() {
        return m_registry.ctx().find<T>();
    }

    template <typename T>
    T& resource() {
        return m_registry.ctx().get<T>();
    }

    entt::registry& registry() {
        return m_registry;
    }

    bool running() const {
        return m_running;
    }
    void quit() {
        m_running = false;
    }

    void run() {}

  private:
    App() = default;
    entt::registry m_registry;
    std::vector<std::unique_ptr<Plugin>> m_plugins;
    bool m_running;
};
} // namespace hm
