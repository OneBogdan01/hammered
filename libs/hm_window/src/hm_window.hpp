#pragma once
#include "plugin.hpp"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

namespace hm {

class WindowPlugin : public Plugin {
  public:
    HM_PLUGIN(Window)
    void build(App& app) override;
};
} // namespace hm
