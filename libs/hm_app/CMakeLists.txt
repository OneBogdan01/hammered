hm_add_module(hm_app)

function(hm_app_link_deps)
    hm_module_deps(hm_app DEPS hm_core)

    CPMAddPackage(
        NAME flecs
        GITHUB_REPOSITORY SanderMertens/flecs
        GIT_TAG v4.1.4
        OPTIONS
            "FLECS_SHARED OFF"
    )
    target_link_libraries(hm_app PUBLIC flecs::flecs_static)

endfunction()

# Allows the application to get the version of the project
configure_file(src/version.hpp.in generated/hm/version.hpp @ONLY)
target_include_directories(hm_app PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated)