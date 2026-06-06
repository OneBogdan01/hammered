#pragma once
namespace hm {
enum class ScheduleLabels {
    // once at the beginning of the app
    Startup,
    // core loop below
    PreUpdate,
    Update,
    PostUpdate,
    // called at fixed intervals
    FixedUpdate,
    Render,
    // once at the end of the app
    Shutdown,

};
class Schedule {};
} // namespace hm
