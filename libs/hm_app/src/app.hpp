#pragma once
#include "plugin.hpp"

#include <flecs.h>
#include <stack>


namespace hm {

enum class Schedule { Startup, PreUpdate, Update, PostUpdate, Shutdown };

using World  = flecs::world;
using Entity = flecs::entity;

class App {
public:
    template <typename P, typename... Args>
    App& add_plugin(Args&&... args) {
        auto p = std::make_unique<P>(std::forward<Args>(args)...);
        p->build(*this);
        m_plugins.push_back(std::move(p));
        return *this;
    }

    template <typename F>
    App& add_systems(Schedule label, F&& fn) {
        if (label == Schedule::Startup) {
            m_startup_fn.emplace_back(
                [this, fn = std::forward<F>(fn)]() { fn(*this); });
            return *this;
        }
        if (label == Schedule::Shutdown) {
            m_shutdown_fn.emplace(
                [this, fn = std::forward<F>(fn)]() { fn(*this); });
            return *this;
        }

        m_world.system()
            .kind(to_flecs_phase(label))
            .run([this, fn = std::forward<F>(fn)](flecs::iter&) { fn(*this); });
        return *this;
    }

    void startup() const;
    void update() const;
    void shutdown();

    World& world() {
        return m_world;
    }

private:
    flecs::entity_t to_flecs_phase(Schedule label) const {
        switch (label) {
        case Schedule::Startup:    return flecs::OnStart;
        case Schedule::PreUpdate:  return flecs::PreUpdate;
        case Schedule::Update:     return flecs::OnUpdate;
        case Schedule::PostUpdate: return flecs::PostUpdate;
        case Schedule::Shutdown:   break;
        }
        return {};
    }


    World m_world;
    std::vector<std::function<void()>> m_startup_fn;
    std::stack<std::function<void()>> m_shutdown_fn;
    std::vector<std::unique_ptr<Plugin>> m_plugins;
};

} // namespace hm