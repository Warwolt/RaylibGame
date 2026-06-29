#pragma once

#include <tracy/Tracy.hpp>

namespace tracy {
	TRACY_API void StartupProfiler();
	TRACY_API void ShutdownProfiler();
} // namespace tracy

#define PROFILING_STARTUP_PROFILER() tracy::StartupProfiler()
#define PROFILING_SHUTDOWN_PROFILER() tracy::ShutdownProfiler()

#define PROFILING_SCOPE() ZoneScoped
#define PROFLING_SCOPE_LABELED(label) ZoneScopedN(label)
#define PROFILING_END_FRAME() FrameMark

// NOTE: `c_str` lifetime must extend until `PROFILING_SHUTDOWN_PROFILER` call
#define PROFILING_LOG(c_str) TracyMessageL(c_str)
