cmake_minimum_required(VERSION 3.24.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HM_VERSION_MAJOR 0)
set(HM_VERSION_MINOR 0)
set(HM_VERSION_PATCH 1)
project(hammered_engine VERSION ${HM_VERSION_MAJOR}.${HM_VERSION_MINOR}.${HM_VERSION_PATCH})

option(HM_BUILD_DEMOS "Build demos" ON)
# Emscripten: output .html so the browser can load it
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

include(cmake/CPM.cmake)
include(cmake/utility.cmake)

add_subdirectory(libs)

if(HM_BUILD_DEMOS)
    add_subdirectory(demos)
endif()