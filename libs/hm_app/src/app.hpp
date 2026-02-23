#pragma once

#include "plugin.hpp"

#include <flecs.h>
#include <functional>
#include <memory>
#include <vector>

namespace hm {

enum class Schedule { Startup, PreUpdate, Update, PostUpdate, Shutdown };

class App {
  public:
    template <typename P, typename... Args>
    App& add_plugin(Args&&... args) {
        auto p = std::make_unique<P>(std::forward<Args>(args)...);
        p->build(*this);
        m_plugins.push_back(std::move(p));
        return *this;
    }

    template <typename T>
    App& insert_resource(T&& r) {
        m_world.set<std::decay_t<T>>(std::forward<T>(r));
        return *this;
    }

    template <typename T>
    const T* get_resource() {
        return &m_world.get<T>();
    }

    template <typename T>
    T* get_resource_mut() {
        return &m_world.ensure<T>();
    }

    App& add_systems(Schedule label, std::function<void(App&)> fn);

    void startup();

    void update() {
        run_schedule(Schedule::PreUpdate);
        run_schedule(Schedule::Update);
        run_schedule(Schedule::PostUpdate);
    }

    void shutdown() {
        run_schedule(Schedule::Shutdown);
    }

    flecs::world& world() {
        return m_world;
    }

  private:
    void run_schedule(Schedule label);

    flecs::world m_world;
    std::vector<std::unique_ptr<Plugin>> m_plugins;
    std::unordered_map<int, std::vector<std::function<void(App&)>>> m_schedules;
};

} // namespace hm
