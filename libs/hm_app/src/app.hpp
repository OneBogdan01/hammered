#pragma once

#include <flecs.h>

namespace hm {

using World = flecs::world;

namespace schedule {
static const auto OnStart = flecs::OnStart;
static const auto PreFrame = flecs::PreFrame;
static const auto OnLoad = flecs::OnLoad;
static const auto PostLoad = flecs::PostLoad;
static const auto PreUpdate = flecs::PreUpdate;
static const auto OnUpdate = flecs::OnUpdate;
static const auto OnValidate = flecs::OnValidate;
static const auto PostUpdate = flecs::PostUpdate;
static const auto PreStore = flecs::PreStore;
static const auto OnStore = flecs::OnStore;
static const auto PostFrame = flecs::PostFrame;
} // namespace schedule

} // namespace hm
