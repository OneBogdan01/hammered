#pragma once
#include "command_buffer.hpp"
#include "pch.hpp"
#include "plugin.hpp"
namespace hm  {

/**
 * Alloy is a signed distance field renderer.
 */
class AlloyPlugin : public Plugin {
  public:
    HM_PLUGIN(AlloyUI)
    void build(App& app) override;

private:
    CommandBuffer m_command_buffer{};

};
} // namespace hm
