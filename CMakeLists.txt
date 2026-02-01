cmake_minimum_required(VERSION 3.24.0)


include(FetchContent)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
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

set (CMAKE_CXX_STANDARD 23)
## Versioning
set( HM_VERSION_MAJOR 0 )
set( HM_VERSION_MINOR 0 )
set( HM_VERSION_PATCH 1 )

set( HM_VERSION ${HM_VERSION_MAJOR}.${HM_VERSION_MINOR}.${HM_VERSION_PATCH})
message(STATUS "Hammered Engine Version: ${HM_VERSION}")

project(hammered_engine VERSION ${HM_VERSION})
# to add backend specific
include(cmake/utility.cmake)


# Backend selection
set(GAME_BACKENDS "")

option(HM_EDITOR "Build editor configuration" OFF)



option(ENABLE_GL_BACKEND "Build with OpenGL backend" ON)
option(ENABLE_VK_BACKEND "Build with Vulkan backend" ON)


if(ENABLE_VK_BACKEND)
    list(APPEND GAME_BACKENDS vk)
endif()
if(ENABLE_GL_BACKEND)
    list(APPEND GAME_BACKENDS gl)
endif()

# Error if no backends enabled
if(NOT GAME_BACKENDS)
    message(FATAL_ERROR "No game backend enabled! Enable at least one of ENABLE_GL_BACKEND or ENABLE_VK_BACKEND")
endif()

# dependency management
include(cmake/CPM.cmake)
# Adds engine code and externals
add_subdirectory(engine)



# Sets up the backends .exes
add_game_backends("${GAME_BACKENDS}")


set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "game_vk")

