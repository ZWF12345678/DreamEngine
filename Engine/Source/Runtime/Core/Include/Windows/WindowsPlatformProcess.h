#pragma once

#include "UniversalPlatform/UniversalPlatformProcess.h"

typedef void* HANDLE;

struct SProcessHandle : public TProcessHandle<HANDLE, nullptr>
{
public:

	FORCEINLINE SProcessHandle()
		: TProcessHandle()
	{}

	FORCEINLINE explicit SProcessHandle( HandleType Other )
		: TProcessHandle( Other )
	{}
};


struct HWindowsPlatformProcess : public HUniversalPlatformProcess
{
    /**
	 * Windows representation of a process semaphore
	 */
    struct HWindowsProcessSemaphore : public HProcessSemaphore
	{
		virtual void	Lock();

		virtual bool	TryLock(UInt64 NanosecondsToWait);

		virtual void	Unlock();

		HANDLE GetSemaphore() { return Semaphore; }

		HWindowsProcessSemaphore(const std::string& InName, HANDLE InSemaphore);

		HWindowsProcessSemaphore(const ANSICHAR* InName, HANDLE InSemaphore);

		virtual ~HWindowsProcessSemaphore();

	protected:

		HANDLE Semaphore;
	};

    static CORE_API HProcessSemaphore* CreateProcessSynchSemaphore(const std::string& Name, bool bCreate, UInt32 MaxLocks = 1);

	static CORE_API HProcessSemaphore* CreateProcessSynchSemaphore(const ANSICHAR* Name, bool bCreate, UInt32 MaxLocks = 1);

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

    /*
	 * Deletes an interprocess synchronization object.
	 */
	static CORE_API bool DeleteProcessSynchSemaphore(HProcessSemaphore* InSemaphore);

    static CORE_API class HRunnableThread* CreateRunnableThread();

    static CORE_API UInt32 GetCurrentProcessId();

    static CORE_API UInt32 GetCurrentCoreNumber();

    static CORE_API void SetThreadAffinityMask(UInt64 InAffinityMask);

    static void SetThreadName(const char* InName);

    static CORE_API HSyncEvent* CreateSynchEvent(bool IsManualReset);

    static CORE_API void ClosePipe( void* ReadPipe, void* WritePipe );

    static CORE_API bool CreatePipe(void*& ReadPipe, void*& WritePipe, bool bWritePipeLocal = false);

    static CORE_API bool ReadPipeToVector(void* ReadPipe, std::vector<UInt8> & Output);

    static CORE_API bool WritePipe(void* WritePipe, const UInt8* Data, const Int32 DataLength, Int32* OutDataLength = nullptr);

    static CORE_API void Sleep( float Seconds );

	static CORE_API void SleepNoStats( float Seconds );

	[[noreturn]] static CORE_API void SleepInfinite();

	static CORE_API void YieldThread();

};

typedef HWindowsPlatformProcess HPlatformProcess;