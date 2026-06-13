
hm_add_module(hm_lua)

function(hm_lua_link_deps)
    hm_module_deps(hm_lua DEPS hm_app)

    CPMAddPackage(
            NAME lua
            GITHUB_REPOSITORY walterschell/Lua
            GIT_TAG v5.4.7
            OPTIONS
            "LUA_BUILD_AS_CXX ON"
            "LUA_ENABLE_SHARED OFF"
            "LUA_ENABLE_TESTING OFF"
            "LUA_BUILD_BINARY OFF"
            "LUA_BUILD_COMPILER OFF"
    )

    CPMAddPackage(
            NAME sol2
            GITHUB_REPOSITORY ThePhD/sol2
            GIT_TAG v3.3.0
    )
# TODO remove public
target_link_libraries(hm_lua PRIVATE
    lua_static
    sol2::sol2
)
target_compile_definitions(hm_lua PRIVATE
    SOL_USING_CXX_LUA=1
)
endfunction()