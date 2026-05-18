#pragma once
#include "colors.hpp"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
namespace hm::alloy {
using fPoint64 = SDL_FPoint;
using fRect128 = SDL_FRect;
enum class ShapeType : u32 { Circle, Line, Rect };

struct Circle {
    fPoint64 center;
    f32 radius;
};
struct Rect {
    fRect128 rect;
};
struct Line {
    fPoint64 a;
    fPoint64 b;
};
// used for all shapes for now
struct alignas(16) UICommand {
    // shape specific
    union {
        Rect rect{};
        Line line;
        Circle circle;
    };

    // 16 bytes as floats
    // In order from the first variables can be interpreted:
    // First 3 are position and radius of circle
    // All make up a rect
    // First 2 make point A and last 2 point B for the line.
    // common to all shapes

    // 4 bytes
    uColor32 color{colors::WHITE};
    // 4 bytes
    f32 shadow_strength = 0.50f;
    ShapeType type = ShapeType::Circle;
    // 4 bytes
    u16 primitive_group = 0u;
    u8 shadow_distance = 2u;
    u8 padding;
    // 4 bytes
    //  TODO add clipping rect
};
static_assert(sizeof(UICommand) == 32);
class UICommandBuffer {
  public:
    UICommandBuffer& add_circle(Circle circle, uColor32 color);
    UICommandBuffer& add_rect(Rect rect, uColor32 color);
    UICommandBuffer& add_line(Line line, uColor32 color);

  private:
    void set_common_values(hm::alloy::uColor32 color, hm::alloy::UICommand& cmd);

    Vec<UICommand> m_commands;
};

} // namespace hm::alloy