option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF) 

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY 
            ${CLANG_TIDY_EXE}
            -p=${CMAKE_BINARY_DIR}
        )
    else()
        message(WARNING "clang-tidy requested but not found")
    endif()
endif()
# clang-format

find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
    
    # Collect all source files (excluding external/)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/engine/*.cpp
        ${CMAKE_SOURCE_DIR}/engine/*.hpp
        ${CMAKE_SOURCE_DIR}/game/*.cpp
        ${CMAKE_SOURCE_DIR}/game/*.hpp
    )
    
    # Filter out external, build, and cache directories
    list(FILTER ALL_CXX_SOURCE_FILES EXCLUDE REGEX ".*external/.*")
    list(FILTER ALL_CXX_SOURCE_FILES EXCLUDE REGEX ".*build/.*")
    list(FILTER ALL_CXX_SOURCE_FILES EXCLUDE REGEX ".*\\.cache/.*")
    
    # Target: format (formats all files)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_CXX_SOURCE_FILES}
        COMMENT "Running clang-format on ${CMAKE_PROJECT_NAME}"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )
    
    # Target: format-check (checks without modifying)
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror -style=file ${ALL_CXX_SOURCE_FILES}
        COMMENT "Checking code formatting"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )
    
    # Print how many files will be formatted
    list(LENGTH ALL_CXX_SOURCE_FILES SOURCE_FILE_COUNT)
    message(STATUS "clang-format will process ${SOURCE_FILE_COUNT} source files")
    
else()
    message(WARNING "clang-format not found - format targets unavailable")
endif()