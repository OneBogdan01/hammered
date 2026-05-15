#pragma once
#include "plugin.hpp"
#include "SDL3/SDL_rect.h"
namespace hm::alloy  {


class AlloyUiPlugin : public Plugin {
  public:
    HM_PLUGIN(AlloyUI)
    void build(App& app) override;
};
} // namespace hm
