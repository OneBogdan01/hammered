#pragma once

#include "plugin.hpp"

#include <memory>
#include <vector>

namespace hm {

class App {
  public:
    template <typename P, typename... Args>
    App& add_plugin(Args&&... args) {
        auto plugin = std::make_unique<P>(std::forward<Args>(args)...);
        plugin->build(*this);
        m_plugins.push_back(std::move(plugin));
        return *this;
    }

    void startup() {}
    void update() {}
    void shutdown() {}

  private:
    std::vector<std::unique_ptr<Plugin>> m_plugins;
};

} // namespace hm
