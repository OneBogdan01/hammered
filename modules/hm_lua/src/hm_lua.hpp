#pragma once
#include "pch.hpp"
#include "plugin.hpp"

namespace sol {
class state;
}

namespace hm {

class LuaPlugin : public Plugin {
  public:
    explicit LuaPlugin(String script_path = "scripts/main.lua");
    ~LuaPlugin() override;
    HM_PLUGIN(Lua)
    void build(App& app) override;

  private:
    void run_script();

    String m_script_path;
    Unique<sol::state> m_lua;
};

} // namespace hm