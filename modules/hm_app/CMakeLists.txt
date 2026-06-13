hm_add_module(hm_app)

function(hm_app_link_deps)
    hm_module_deps(hm_app DEPS hm_core)
    CPMAddPackage(
            NAME SDL3
            GITHUB_REPOSITORY libsdl-org/SDL
            GIT_TAG release-3.4.2
            OPTIONS
            "SDL_SHARED OFF"
            "SDL_STATIC ON"
    )
    target_link_libraries(hm_app PUBLIC SDL3::SDL3-static)

endfunction()

# Allows the application to get the version of the project
configure_file(src/version.hpp.in generated/hm/version.hpp @ONLY)
target_include_directories(hm_app PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated)