#pragma once

#include <vector>
#include <functional>
#include <string>

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

#if PLATFORM_APPLE
#include <mach/mach_time.h>
#endif

#if !defined(__clang__)
#include <intrin.h>
#if defined(_M_ARM)
#include <armintr.h>
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
#include <arm64intr.h>
#endif
#endif

class HSyncEvent;
struct SProcessHandle;

/** Universal template for the process handle. */
template< typename T, T InvalidHandleValue >
struct TProcessHandle
{
	typedef T HandleType;
public:

	FORCEINLINE TProcessHandle()
		: Handle( InvalidHandleValue )
	{ }

	FORCEINLINE explicit TProcessHandle( T Other )
		: Handle( Other )
	{ }

	FORCEINLINE T Get() const
	{
		return Handle;
	}

	FORCEINLINE void Reset()
	{
		Handle = InvalidHandleValue;
	}

	FORCEINLINE bool IsValid() const
	{
		return Handle != InvalidHandleValue;
	}

protected:
	
	T Handle;
};


struct HUniversalPlatformProcess
{

	/**
	 * Universal representation of a interprocess semaphore
	 */
	struct HProcessSemaphore
	{
		const ANSICHAR* GetName() const
		{
			return Name;
		}

		virtual void Lock() = 0;

		virtual bool TryLock(UInt64 NanosecondsToWait) = 0;

		virtual void Unlock() = 0;

		HProcessSemaphore(const std::string& InName);

		HProcessSemaphore(const ANSICHAR* InName);

		virtual ~HProcessSemaphore() { };

	protected:

		enum Limits
		{
			MaxSemaphoreName = 128
		};

		ANSICHAR	Name[MaxSemaphoreName];
	};

#if PLATFORM_HAS_BSD_TIME 

	static CORE_API void Sleep( float Seconds );

	static CORE_API void SleepNoStats( float Seconds );

	[[noreturn]] static CORE_API void SleepInfinite();

	static CORE_API void YieldThread();

#endif 

	static CORE_API void SleepCondition(std::function<bool()> Condition, float SleepTime = 0.0f);

	/*
	 * Opens a process handle for the given process ID.
	 * @param ProcessID The ID of the process to open.
	 * @return A handle to the opened process, or an invalid handle if the operation failed.
	 * @note This function is not thread-safe and should be used with caution.
	*/
	static CORE_API SProcessHandle OpenProcess(UInt32 ProcessID);

	static CORE_API void CloseProcess( SProcessHandle & ProcessHandle );

	static CORE_API bool ProcessIsRunning( SProcessHandle & ProcessHandle );

	static CORE_API void WaitForProcess( SProcessHandle & ProcessHandle );

	static CORE_API void TerminateProcess( SProcessHandle & ProcessHandle, bool KillTree = false );

	static CORE_API void TerminateProcessTreeWithPredicate(SProcessHandle& ProcessHandle,std::function<bool(UInt32 ProcessId, const ANSICHAR* ApplicationName)> Predicate);

	static CORE_API bool GetProcessReturnCode( SProcessHandle & ProcHandle, Int32& ReturnCode );

	static CORE_API bool ApplicationIsRunning( UInt32 ProcessId );

	static CORE_API std::string GetApplicationName( UInt32 ProcessId );

	static CORE_API bool GetApplicationMemoryUsed(UInt32 ProcessId, SIZE_T& OutMemoryUsedSize);

	static CORE_API bool ExecProcess(const ANSICHAR* URL, const ANSICHAR* Params, Int32& OutReturnCode, std::string& OutStdOut, std::string& OutStdErr, const ANSICHAR* OptionalWorkingDirectory = NULL, bool bShouldEndWithParentProcess = false);

	static CORE_API bool ExecAdminProcess(const ANSICHAR* URL, const ANSICHAR* Params, Int32& OutReturnCode);

	static CORE_API bool SupportsMultithreading();

	static CORE_API class HRunnableThread* CreateRunnableThread();

	static CORE_API UInt32 GetCurrentProcessId();

	static CORE_API UInt32 GetCurrentCoreNumber();

	static CORE_API void SetThreadAffinityMask(UInt64 InAffinityMask);

	static CORE_API void SetThreadPriority(UInt32 InPriority);

	static void SetThreadName(const char* InName);

	static CORE_API UInt32 GetStackSize();

	static CORE_API bool ShouldSaveToUserDir();

	static CORE_API HSyncEvent* CreateSynchEvent(bool IsManualReset);

	static CORE_API HSyncEvent* GetSynchEventFromPool(bool IsManualReset = false);

	static CORE_API void FlushSynchEventPool();

	static CORE_API void ReturnSynchEventToPool(HSyncEvent* InEvent);

	static CORE_API void ClosePipe( void* ReadPipe, void* WritePipe );

	static CORE_API bool CreatePipe(void*& ReadPipe, void*& WritePipe, bool bWritePipeLocal = false);

	// static CORE_API String ReadPipe( void* ReadPipe );

	// static CORE_API bool WritePipe(void* WritePipe, const String& Message, String* OutWritten = nullptr);

	static CORE_API bool ReadPipeToVector(void* ReadPipe, std::vector<UInt8> & Output);

	static CORE_API bool WritePipe(void* WritePipe, const UInt8* Data, const Int32 DataLength, Int32* OutDataLength = nullptr);

	/*
	 *Creates or opens an interprocess synchronization object.
	*/
	static CORE_API HProcessSemaphore* 	CreateProcessSynchSemaphore(const std::string& Name, bool bCreate, UInt32 MaxLocks = 1);

	static CORE_API HProcessSemaphore*  CreateProcessSynchSemaphore(const ANSICHAR* Name, bool bCreate, UInt32 MaxLocks = 1);

	/*
	 * Deletes an interprocess synchronization object.
	 */
	static CORE_API bool DeleteProcessSynchSemaphore(HProcessSemaphore* InSemaphore);

	static FORCEINLINE void Yield()
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
#	error Unsupported architecture!
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
			Yield();
		}
#endif
	}
};
