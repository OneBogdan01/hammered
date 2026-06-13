#include "hm_lua.hpp"
#include "app.hpp"
#include "hm_log.hpp"

#include <sol/sol.hpp>

namespace hm {

LuaPlugin::LuaPlugin(String script_path)
    : m_script_path(std::move(script_path)), m_lua(std::make_unique<sol::state>()) {
    m_lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string,
                          sol::lib::table, sol::lib::os);

    sol::table hm_tbl = m_lua->create_named_table("hm");
    hm_tbl.set_function("log", [](const std::string& msg) { log::info("[lua] {}", msg); });
}

LuaPlugin::~LuaPlugin() = default;


void LuaPlugin::build(App& app) {
    app.add_systems(Schedule::Startup, [this](App&) { run_script(); });
}

void LuaPlugin::run_script() {
    sol::protected_function_result r =
        m_lua->safe_script_file(m_script_path, sol::script_pass_on_error);
    if (!r.valid()) {
        const sol::error err = r;
        log::error("Lua [{}]: {}", m_script_path, err.what());
        return;
    }
    log::info("Lua script '{}' loaded", m_script_path);
}

} // namespace hm