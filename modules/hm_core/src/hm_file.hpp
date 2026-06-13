#pragma once
#include "pch.hpp"

#include <SDL3/SDL_filesystem.h>
namespace hm {
static StringView root_folder() {
    return StringView(SDL_GetBasePath());
}
} // namespace hm