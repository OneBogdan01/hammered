
#include "command_buffer_ui.hpp"

void hm::alloy::UICommandBuffer::set_common_values(hm::alloy::uColor32 color,
                                                   hm::alloy::UICommand& cmd) {
    cmd.color = color;
    cmd.type = ShapeType::Circle;
}
hm::alloy::UICommandBuffer& hm::alloy::UICommandBuffer::add_circle(Circle circle, uColor32 color) {
    UICommand cmd{};
    set_common_values(color, cmd);
    cmd.circle = circle;
    m_commands.push_back(cmd);
    return *this;
}