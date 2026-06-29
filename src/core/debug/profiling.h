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
