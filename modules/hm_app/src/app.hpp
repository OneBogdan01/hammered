#pragma once
#include "plugin.hpp"
#include "schedule.hpp"

#include <stack>

namespace hm {

class App {
  public:
    App();
    template <typename P, typename... Args>
    App& add_plugin(Args&&... args) {
        auto p = std::make_unique<P>(std::forward<Args>(args)...);
        p->build(*this);
        m_plugins.push_back(std::move(p));
        return *this;
    }

    template <typename F>
    App& add_systems(Schedule label, F&& fn) {
        auto wrap = [this, fn = std::forward<F>(fn)]() { fn(*this); };
        switch (label) {
        case Schedule::Startup:
            m_startup_fn.emplace_back(std::move(wrap));
            break;
        case Schedule::Shutdown:
            m_shutdown_fn.emplace_back(std::move(wrap));
            break;
        case Schedule::PreUpdate:
            break;
        case Schedule::Update:
            m_update_fn.emplace_back(std::move(wrap));
            break;
        case Schedule::PostUpdate:
            break;
        case Schedule::FixedUpdate:
            break;
        }
        return *this;
    }
    template <std::derived_from<Plugin> PluginType>
    Opt<std::reference_wrapper<PluginType>> get_plugin_mutable() {
        for (auto& p : m_plugins) {
            if (auto* concrete = dynamic_cast<PluginType*>(p.get())) {
                return std::ref(*concrete);
            }
        }
        return std::nullopt;
    }

    template <std::derived_from<Plugin> PluginType>
    Opt<std::reference_wrapper<const PluginType>> get_plugin() const {
        for (const auto& p : m_plugins) {
            if (const auto* concrete = dynamic_cast<const PluginType*>(p.get())) {
                return std::cref(*concrete);
            }
        }
        return std::nullopt;
    }
    void startup() const;
    void update() const;
    void shutdown() const;

  private:
    std::vector<std::function<void()>> m_startup_fn;
    std::vector<std::function<void()>> m_shutdown_fn;
    std::vector<std::function<void()>> m_update_fn;

    std::vector<std::unique_ptr<Plugin>> m_plugins;
};

} // namespace hm