#pragma once
#include "types.hpp"

namespace hm {
struct Window {
    u32 width = 0u;
    u32 height = 0u;
    std::string title{"Default Window Name"};
};
} // namespace hm
