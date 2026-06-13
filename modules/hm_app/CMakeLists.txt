hm_add_module(hm_app)

function(hm_app_link_deps)
    hm_module_deps(hm_app DEPS hm_core)
endfunction()

# Allows the application to get the version of the project
configure_file(src/version.hpp.in generated/hm/version.hpp @ONLY)
target_include_directories(hm_app PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated)