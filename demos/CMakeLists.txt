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
            set_target_properties(demo_${demo} PROPERTIES SUFFIX ".html")
            
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
            OUTPUT_NAME "index"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/demos/${demo}"
        )
    endif()
endforeach()