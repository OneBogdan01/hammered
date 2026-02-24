#pragma once

#include <flecs.h>

namespace hm {

using World = flecs::world;

class App {
  public:
    const World& get_world() {
        return m_world;
    }
    World& get_world_mut() {
        return m_world;
    }

  private:
    World m_world;
};

} // namespace hm
