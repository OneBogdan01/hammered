#pragma once
#include "plugin.hpp"

namespace hm {

class WindowPlugin : public Plugin {
  public:
    HM_PLUGIN(Window)
    void build(App& app) override;

};
} // namespace hm
