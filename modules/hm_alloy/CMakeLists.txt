
hm_add_module(hm_alloy)

function(hm_alloy_link_deps)
    hm_module_deps(hm_alloy DEPS hm_app hm_window)
    # External deps
    CPMAddPackage(
    NAME SDL3
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG release-3.4.2 
    OPTIONS
        "SDL_SHARED OFF"
        "SDL_STATIC ON"
)

target_link_libraries(hm_alloy PUBLIC SDL3::SDL3-static)
endfunction()