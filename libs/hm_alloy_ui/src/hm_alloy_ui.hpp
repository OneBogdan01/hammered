#pragma once
#include "plugin.hpp"


namespace hm {

class AlloyUi : public Plugin {
public:
    HM_PLUGIN(AlloyUI)
    void build(App& app) override;
};
} // namespace hm
