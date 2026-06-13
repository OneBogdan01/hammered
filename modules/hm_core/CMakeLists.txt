
hm_add_module(hm_core)
target_precompile_headers(hm_core PUBLIC src/pch.hpp)
function(hm_core_link_deps)
    CPMAddPackage(
            NAME SDL3
            GITHUB_REPOSITORY libsdl-org/SDL
            GIT_TAG release-3.4.2
            OPTIONS
            "SDL_SHARED OFF"
            "SDL_STATIC ON"
    )
    target_link_libraries(hm_core PUBLIC SDL3::SDL3-static)
endfunction()