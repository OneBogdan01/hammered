file(GLOB demo_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach(demo ${demo_dirs})
    set(demo_dir ${CMAKE_CURRENT_SOURCE_DIR}/${demo})
    
    if(IS_DIRECTORY ${demo_dir} AND EXISTS ${demo_dir}/src/main.cpp)
        message(STATUS "Adding demo: ${demo}")
        
        file(GLOB_RECURSE DEMO_SOURCES CONFIGURE_DEPENDS
            ${demo_dir}/*.cpp
            ${demo_dir}/*.hpp
        )
        
        add_executable(demo_${demo} ${DEMO_SOURCES})
        target_link_libraries(demo_${demo} PRIVATE hammered-engine)
        
        if(EMSCRIPTEN)
            set_target_properties(demo_${demo} PROPERTIES
                    SUFFIX ".html"
                    OUTPUT_NAME "index"
            )

            # Shared flags
            target_link_options(demo_${demo} PRIVATE
                -sALLOW_MEMORY_GROWTH=1 
                -sSTACK_SIZE=1mb
                -sEXPORTED_RUNTIME_METHODS=cwrap 
                --shell-file ${CMAKE_SOURCE_DIR}/shell.html
            )
            # Debug-only flags
            if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                target_link_options(demo_${demo} PRIVATE
                    -sASSERTIONS=2
                    -sSTACK_OVERFLOW_CHECK=2
                    -sSAFE_HEAP=1
                    -sGL_ASSERTIONS=1
                )
            endif()
        endif()

        set_target_properties(demo_${demo} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/demos/${demo}"
        )

        if(IS_DIRECTORY ${demo_dir}/assets)
            add_custom_target(demo_${demo}_copy_assets ALL
                    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
                    ${demo_dir}/assets
                    $<TARGET_FILE_DIR:demo_${demo}>/assets
                    COMMENT "Copying [${demo}] assets"
            )
            add_dependencies(demo_${demo} demo_${demo}_copy_assets)
        endif()
    endif()
endforeach()