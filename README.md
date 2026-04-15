# Hammered 

This is a mainly an educational engine for implementing various low-level systems and learning graphics programming in a cross-platform fashion.

Built with **C++23**, **SDL3**, and **CMake**.

## Articles

I write articles about topics I explore in Hammered. Each article links to the version of the code it describes.

| Article | Code |
|---|---|
| [CMake Setup](https://tycro-games.github.io/posts/Hammered-Cross-Platform-Game-Engine-CMake-Setup/) — CMake build system with OpenGL and Vulkan backends| [Browse code (branch)](https://github.com/OneBogdan01/hammered/tree/cmake-opengl-vulkan-set-up) |
| [Learning Multithreading With a Logger](https://tycro-games.github.io/posts/Learning-Multithreading-With-A-Logger/) — Async logging using a lock-free queue and a dedicated writer thread *(Vulkan backend only)* | [Browse code (commit)](https://github.com/OneBogdan01/hammered/tree/b7f02d68582e81e534c6a6183b8724575e26d28f) |

> **Tip:** The code links point to snapshots of the project at the time each article was written. The `main` branch reflects the latest state of the engine.

The async logger snapshot includes [Tracy](https://github.com/wolfpld/tracy) profiler integration. To enable it, configure with `RelWithDebInfo`:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
```

## Building

```bash
git clone --recursive https://github.com/OneBogdan01/hammered.git
cd hammered
git submodule update --init
cmake -B build
cmake --build build
```

The executable will be in `build/bin/Debug/`.

> **Note:** Downloading the ZIP from GitHub won't work, submodules are not included in ZIP downloads. You need to clone the repo with git. I plan to fix this in the future using CMake Fetch.

## Status

This project is actively in development. I am currently restructuring it toward a modular architecture.

[![Build](https://github.com/OneBogdan01/hammered-engine/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/OneBogdan01/hammered-engine/actions/workflows/cmake-multi-platform.yml)
