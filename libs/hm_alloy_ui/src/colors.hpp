#pragma once
#include "SDL3/SDL_pixels.h"
namespace hm::alloy {
using Color = SDL_Color;
}
namespace hm::alloy::colors {

// Grayscale
constexpr Color BLACK{0, 0, 0, 255};
constexpr Color WHITE{255, 255, 255, 255};
constexpr Color GRAY{128, 128, 128, 255};
constexpr Color LIGHT_GRAY{200, 200, 200, 255};
constexpr Color DARK_GRAY{64, 64, 64, 255};
constexpr Color TRANSPARENT{0, 0, 0, 0};

// Primaries
constexpr Color RED{255, 0, 0, 255};
constexpr Color GREEN{0, 255, 0, 255};
constexpr Color BLUE{0, 0, 255, 255};

// Secondaries
constexpr Color YELLOW{255, 255, 0, 255};
constexpr Color CYAN{0, 255, 255, 255};
constexpr Color MAGENTA{255, 0, 255, 255};

} // namespace hm::alloy::colors
