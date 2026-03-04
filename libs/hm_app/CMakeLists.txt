hm_add_module(hm_app)

function(hm_app_link_deps)
    hm_module_deps(hm_app DEPS hm_core)

    CPMAddPackage(
        NAME flecs
        GITHUB_REPOSITORY SanderMertens/flecs
        GIT_TAG master
    )
    target_link_libraries(hm_app PUBLIC flecs::flecs_static)

endfunction()