#pragma once
#include "plugin.hpp"
namespace hm {

class CompatibleRendererPlugin : public Plugin {
  public:
    HM_PLUGIN(CompatibleRenderer);
    void build(App& app) override;
};
} // namespace hm
