add_library(hammered-engine INTERFACE)

target_include_directories(hammered-engine 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(hammered-engine 
    INTERFACE ${HM_ALL_MODULES}
)