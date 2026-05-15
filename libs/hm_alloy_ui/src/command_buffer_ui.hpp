#pragma once
#include "colors.hpp"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
namespace hm::alloy {
using Point = SDL_Point;

// shared by all commands
struct UICommand {
    enum class Type : u8 { Circle, Line, Rect };
    Type type = Type::Circle;
    u8 shadow_distance = 2u;
    u16 primitive_group = 0u;
    f32 shadow_strength = 0.50f;
    //TODO add clipping rect
};
class UICommandBuffer {
  public:
    void AddCircle();

  private:
    Vec<UICommand> m_commands;
};
struct CircleCommand {
    UICommand m_command{};
    Point m_center{};
    f32 m_radius = 1.0f;
    Color m_color{colors::WHITE};
};
} // namespace hm::alloy