#include "app.hpp"
hm::App& hm::App::add_systems(Schedule label, std::function<void(App&)> fn) {
    m_schedules[static_cast<int>(label)].push_back(std::move(fn));
    return *this;
}
void hm::App::startup() {
    run_schedule(Schedule::Startup);
}
void hm::App::run_schedule(Schedule label) {
    auto it = m_schedules.find(static_cast<int>(label));
    if (it == m_schedules.end())
        return;
    for (auto& sys : it->second)
        sys(*this);
}
