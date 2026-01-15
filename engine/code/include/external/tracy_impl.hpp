#pragma once

#include <tracy/Tracy.hpp>

#ifndef HM_CALLSTACK_DEPTH
#define HM_CALLSTACK_DEPTH 16
#endif
// Zone scoping
#define HM_ZONE_SCOPED ZoneScoped
#define HM_ZONE_SCOPED_N(name) ZoneScopedN(name)
#define HM_ZONE_SCOPED_C(color) ZoneScopedC(color)
#define HM_ZONE_SCOPED_NC(name, color) ZoneScopedNC(name, color)
#define HM_ZONE_TEXT(text, len) ZoneText(text, len)
#define HM_ZONE_VALUE(value) ZoneValue(value)

// Frame marks
#define HM_FRAME_MARK FrameMark
#define HM_FRAME_MARK_N(name) FrameMarkNamed(name)
#define HM_FRAME_MARK_START(name) FrameMarkStart(name)
#define HM_FRAME_MARK_END(name) FrameMarkEnd(name)

// Plotting values over time
#define HM_PLOT(name, val) TracyPlot(name, val)
#define HM_PLOT_CONFIG(name, type, step, fill, color) \
  TracyPlotConfig(name, type, step, fill, color)

// Messages/logging
#define HM_MESSAGE(text, len) TracyMessage(text, len)
#define HM_MESSAGE_L(text) TracyMessageL(text)
#define HM_MESSAGE_C(text, len, color) TracyMessageC(text, len, color)

// Memory tracking
#define HM_ALLOC(ptr, size) TracyAlloc(ptr, size)
#define HM_FREE(ptr) TracyFree(ptr)

// Lock profiling
#define HM_LOCKABLE(type, var) TracyLockable(type, var)
#define HM_LOCKABLE_N(type, var, name) TracyLockableN(type, var, name)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4595)

#endif

inline void* operator new(std::size_t count)
{
  auto ptr = malloc(count);
  TracyAllocS(ptr, count, HM_CALLSTACK_DEPTH);
  return ptr;
}
inline void operator delete(void* ptr) noexcept
{
  TracyFreeS(ptr, HM_CALLSTACK_DEPTH);
  free(ptr);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
