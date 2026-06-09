file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
set(HM_ALL_MODULES "")

foreach(child ${children})
    if(child STREQUAL "hammered_engine")
        continue()
    endif()
    
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} 
       AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
        message(STATUS "Adding module: ${child}")
        add_subdirectory(${child})
        list(APPEND HM_ALL_MODULES ${child})
    endif()
endforeach()

foreach(module ${HM_ALL_MODULES})
    if(COMMAND ${module}_link_deps)
        cmake_language(CALL ${module}_link_deps)
    endif()
endforeach()

set(HM_ALL_MODULES ${HM_ALL_MODULES} CACHE INTERNAL "All HM modules")

# Add umbrella last
add_subdirectory(hammered_engine)