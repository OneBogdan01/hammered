
#include "command_buffer.hpp"

hm::alloy::UICommand::UICommand(Circle circle, uColor32 col)
    : circle{circle}, color{col}, type{ShapeType::Circle} {}

hm::alloy::UICommand::UICommand(Rect rect, uColor32 col)
    : rect{rect}, color{col}, type{ShapeType::Rect} {}

hm::alloy::UICommand::UICommand(Line line, uColor32 col)
    : line{line}, color{col}, type{ShapeType::Line} {}
hm::alloy::UICommandBuffer& hm::alloy::UICommandBuffer::add_circle(const Circle circle,
                                                                   const uColor32 color) {
    const UICommand cmd{circle, color};
    m_commands.push_back(cmd);
    return *this;
}
hm::alloy::UICommandBuffer& hm::alloy::UICommandBuffer::add_rect(const Rect rect,
                                                                 const uColor32 color) {
    const UICommand cmd{rect, color};
    m_commands.push_back(cmd);
    return *this;
}
hm::alloy::UICommandBuffer& hm::alloy::UICommandBuffer::add_line(const Line line,
                                                                 const uColor32 color) {
    const UICommand cmd{line, color};
    m_commands.push_back(cmd);
    return *this;
}
hm::Vec<hm::alloy::UICommand>& hm::alloy::UICommandBuffer::get_commands() {
    return m_commands;
}