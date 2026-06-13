#pragma once
#include "command_buffer.hpp"
#include "pch.hpp"
#include "plugin.hpp"
namespace hm {

struct AlloyConfig {
    u64 max_number_shapes{1'048'576u};
};
/**
 * Alloy is a signed distance field renderer.
 */
class AlloyPlugin : public Plugin {
  public:
    AlloyPlugin(AlloyConfig cfg = {});
    HM_PLUGIN(AlloyUI) void build(App& app) override;

  private:
    CommandBuffer m_command_buffer{};
};
} // namespace hm
