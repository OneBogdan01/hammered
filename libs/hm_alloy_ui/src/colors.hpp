#pragma once
#include "SDL3/SDL_pixels.h"
namespace hm::alloy {
using uColor32 = SDL_Color;
using fColor128 = SDL_FColor;
}
namespace hm::alloy::colors {

// Grayscale
constexpr uColor32 BLACK{0, 0, 0, 255};
constexpr uColor32 WHITE{255, 255, 255, 255};
constexpr uColor32 GRAY{128, 128, 128, 255};
constexpr uColor32 LIGHT_GRAY{200, 200, 200, 255};
constexpr uColor32 DARK_GRAY{64, 64, 64, 255};
constexpr uColor32 TRANSPARENT{0, 0, 0, 0};

// Primaries
constexpr uColor32 RED{255, 0, 0, 255};
constexpr uColor32 GREEN{0, 255, 0, 255};
constexpr uColor32 BLUE{0, 0, 255, 255};

// Secondaries
constexpr uColor32 YELLOW{255, 255, 0, 255};
constexpr uColor32 CYAN{0, 255, 255, 255};
constexpr uColor32 MAGENTA{255, 0, 255, 255};

} // namespace hm::alloy::colors
