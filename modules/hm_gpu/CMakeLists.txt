
hm_add_module(hm_gpu)

function(hm_gpu_link_deps)
    hm_module_deps(hm_gpu DEPS hm_window)

endfunction()