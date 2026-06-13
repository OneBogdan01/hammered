#pragma once
#include "SDL3/SDL_pixels.h"
namespace hm {
using uColor32 = SDL_Color;
using fColor128 = SDL_FColor;
} // namespace hm
namespace hm::alloy::colors {
constexpr fColor128 from_u8(uColor32 c) {
    return {
        c.r / 255.0f,
        c.g / 255.0f,
        c.b / 255.0f,
        c.a / 255.0f,
    };
}
namespace u8 {
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
} // namespace u8
namespace f128 {
// Grayscale
constexpr fColor128 BLACK = from_u8(u8::BLACK);
constexpr fColor128 WHITE = from_u8(u8::WHITE);
constexpr fColor128 GRAY = from_u8(u8::GRAY);
constexpr fColor128 LIGHT_GRAY = from_u8(u8::LIGHT_GRAY);
constexpr fColor128 DARK_GRAY = from_u8(u8::DARK_GRAY);
constexpr fColor128 TRANSPARENT = from_u8(u8::TRANSPARENT);

// Primaries
constexpr fColor128 RED = from_u8(u8::RED);
constexpr fColor128 GREEN = from_u8(u8::GREEN);
constexpr fColor128 BLUE = from_u8(u8::BLUE);

// Secondaries
constexpr fColor128 YELLOW = from_u8(u8::YELLOW);
constexpr fColor128 CYAN = from_u8(u8::CYAN);
constexpr fColor128 MAGENTA = from_u8(u8::MAGENTA);
} // namespace f128

} // namespace hm::alloy::colors
