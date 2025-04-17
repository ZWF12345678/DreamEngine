#pragma once

#if defined(__MINGW32__) || defined(__GNUC__)
#pragma GCC target("waitpkg")
#endif

#include "CoreType.h"
#include "Common/PlatformMisc.h"

#include <emmintrin.h>
#include <intrin.h>

#if PLATFORM_CPU_X86_FAMILY
#include <immintrin.h>
#endif


class HSyncEvent;

struct HUniversalPlatformProcess
{
	static CORE_API bool SupportsMultithreading();

	static CORE_API class HRunnableThread* CreateRunnableThread();

	static CORE_API UInt32 GetCurrentProcessId();

	static CORE_API UInt32 GetCurrentCoreNumber();

	static CORE_API void SetThreadAffinityMask(UInt64 InAffinityMask);

	static CORE_API void SetThreadPriority(UInt32 InPriority);

	static void SetThreadName(const char* InName);

	static CORE_API UInt32 GetStackSize();

	static CORE_API bool ShouldSaveToUserDir();

	static CORE_API HSyncEvent* CreateSynchEvent(bool IsManualReset = false);

	static CORE_API HSyncEvent* GetSynchEventFromPool(bool IsManualReset = false);

	static CORE_API void FlushSynchEventPool();

	static CORE_API void ReturnSynchEventToPool(HSyncEvent* InEvent);


	static FORCEINLINE void YieldThread()
	{
#if PLATFORM_USE_SSE2_FOR_THREAD_YIELD
		_mm_pause();
#elif PLATFORM_CPU_ARM_FAMILY

#if  !defined(__clang__)
		__yield(); // MSVC
#else
		__builtin_arm_yield();
#endif
#endif
	}

	static FORCEINLINE void YieldCycles(UInt64 Cycles)
	{
#if PLATFORM_CPU_X86_FAMILY
		auto ReadCycleCounter = []()
		{
#if	(defined(_MSC_VER) || defined(__MINGW32__))
			return __rdtsc();
#elif PLATFORM_APPLE
			return mach_absolute_time();
#elif __has_builtin(__builtin_readcyclecounter)
			return __builtin_readcyclecounter();
#else
	// the platform with other architectures must override this to not have this function be called
	unimplemented();
#endif
		};

		UInt64 start = ReadCycleCounter();
		//some 32bit implementations return 0 for __builtin_readcyclecounter just to be on the safe side we protect against this.
		Cycles = start != 0 ? Cycles : 0;

#if PLATFORM_WINDOWS
		if (HPlatformMisc::HasTimedPauseCPUFeature())
		{
			UInt64 PauseCycles = ReadCycleCounter() + Cycles;
#if	(defined(_MSC_VER) || defined(__MINGW32__))
			_tpause(0, PauseCycles);
#elif __has_builtin(__builtin_ia32_tpause)
			__builtin_ia32_tpause(0, (UInt32)(PauseCycles >> 32), (UInt32)PauseCycles);
#else
#	error Unsupported architecture!
#endif
		}
		else
#endif
		{
			do
			{
				YieldThread();
			} while ((ReadCycleCounter() - start) < Cycles);
		}

#else
		// We can't read cycle counter from user mode on these platform
		for (UInt64 i = 0; i < Cycles; i++)
		{
			YieldThread();
		}
#endif
	}
};
