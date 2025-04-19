#include "Math/MathUtility.h"
#include "Template/TSingleton.h"
#include "Common/SyncEventPool.h"
#include "Common/PlatformProcess.h"
#include "UniversalPlatform/UniversalPlatformProcess.h"

#if PLATFORM_HAS_BSD_TIME 
	#include <unistd.h>
	#include <sched.h>
#endif
#include <string.h>


#if PLATFORM_HAS_BSD_TIME 

void HUniversalPlatformProcess::Sleep( float Seconds )
{
	SleepNoStats(Seconds);
}

void HUniversalPlatformProcess::SleepNoStats( float Seconds )
{
	const Int32 usec = HMath::TruncToInt(Seconds * 1000000.0f);
	if (usec > 0)
	{
		usleep(usec);
	}
	else
	{
		sched_yield();
	}
}
void HUniversalPlatformProcess::SleepInfinite()
{
	while (true)
	{
		pause();
	}
}

void HUniversalPlatformProcess::YieldThread()
{
	sched_yield();
}

#endif

void HUniversalPlatformProcess::SleepCondition(std::function<bool()> Condition, float SleepTime)
{
	if(Condition())
	{
		return;
	}

	do
	{
		HPlatformProcess::SleepNoStats(SleepTime);
	} while (!Condition());
	
}

SProcessHandle HUniversalPlatformProcess::OpenProcess(UInt32 ProcessID)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::OpenProcess not implemented on this platform");

	return SProcessHandle();
}

void HUniversalPlatformProcess::CloseProcess( SProcessHandle & ProcessHandle )
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::CloseProcess not implemented on this platform");
}

bool HUniversalPlatformProcess::ProcessIsRunning( SProcessHandle & ProcessHandle )
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::ProcessIsRunning not implemented on this platform");
	return false;
}

void HUniversalPlatformProcess::WaitForProcess( SProcessHandle & ProcessHandle )
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::WaitForProcess not implemented on this platform");
}

void HUniversalPlatformProcess::TerminateProcess( SProcessHandle & ProcessHandle, bool KillTree)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::TerminateProcess not implemented on this platform");
}

void HUniversalPlatformProcess::TerminateProcessTreeWithPredicate(SProcessHandle& ProcessHandle,std::function<bool(UInt32 ProcessId, const ANSICHAR* ApplicationName)> Predicate)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::TerminateProcessTreeWithPredicate not implemented on this platform");
}

bool HUniversalPlatformProcess::GetProcessReturnCode( SProcessHandle& ProcHandle, Int32& ReturnCode )
{
	 //TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::GetProcessReturnCode not implemented on this platform");
	return false;
}

bool HUniversalPlatformProcess::ApplicationIsRunning( UInt32 ProcessId )
{
	 //TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::ApplicationIsRunning not implemented on this platform");
	return false;
}

std::string HUniversalPlatformProcess::GetApplicationName( UInt32 ProcessId )
{
	 //TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::GetApplicationName not implemented on this platform");

	return std::string("");
}

bool HUniversalPlatformProcess::GetApplicationMemoryUsed(UInt32 ProcessId, SIZE_T& OutMemoryUsedSize)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::GetApplicationMemoryUsed not implemented on this platform");

	return false;
}

bool HUniversalPlatformProcess::ExecProcess(const ANSICHAR* URL, const ANSICHAR* Params, Int32& OutReturnCode, std::string& OutStdOut, std::string& OutStdErr, const ANSICHAR* OptionalWorkingDirectory , bool bShouldEndWithParentProcess)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::ExecProcess not implemented on this platform");

	return false;
}

bool HUniversalPlatformProcess::ExecAdminProcess(const ANSICHAR* URL, const ANSICHAR* Params, Int32& OutReturnCode)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::ExecAdminProcess not implemented on this platform");

	return false;
}

bool HUniversalPlatformProcess::SupportsMultithreading()
{
	static bool bSupportsMultithreading = true;

	return bSupportsMultithreading;
}

HRunnableThread* HUniversalPlatformProcess::CreateRunnableThread()
{
#if PLATFORM_USE_PTHREADS
	return new HRunnableThreadPThread();
#else
    //TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::CreateRunnableThread not implemented on this platform");
	return nullptr;
#endif
}

UInt32 HUniversalPlatformProcess::GetCurrentProcessId()
{
	return 0;
}

UInt32 HUniversalPlatformProcess::GetCurrentCoreNumber()
{
	return 0;
}

void HUniversalPlatformProcess::SetThreadAffinityMask(UInt64 InAffinityMask)
{

}

void HUniversalPlatformProcess::SetThreadPriority(UInt32 InPriority)
{

}

void HUniversalPlatformProcess::SetThreadName(const char* InName)
{

}

UInt32 HUniversalPlatformProcess::GetStackSize()
{
	return 0;
}

bool HUniversalPlatformProcess::ShouldSaveToUserDir()
{
	return false;
}

HSyncEvent* HUniversalPlatformProcess::CreateSynchEvent(bool IsManualReset)
{
	//TODO: Implement this
	return nullptr;
}


HSyncEvent* HUniversalPlatformProcess::GetSynchEventFromPool(bool IsManualReset)
{
	return IsManualReset ? Singleton::GetInstance<HSyncEventPool<ESyncEventMode::ManualReset>>().GetSyncEventFromPool() :
		Singleton::GetInstance<HSyncEventPool<ESyncEventMode::AutoReset>>().GetSyncEventFromPool();
}

void HUniversalPlatformProcess::FlushSynchEventPool()
{
	Singleton::GetInstance<HSyncEventPool<ESyncEventMode::ManualReset>>().EmptyPool();

	Singleton::GetInstance<HSyncEventPool<ESyncEventMode::AutoReset>>().EmptyPool();
}

void HUniversalPlatformProcess::ReturnSynchEventToPool(HSyncEvent* InEvent)
{
	if (InEvent)
	{
		if (InEvent->IsManualReset())
		{
			Singleton::GetInstance<HSyncEventPool<ESyncEventMode::ManualReset>>().ReturnToPool(InEvent);
		}
		else
		{
			Singleton::GetInstance<HSyncEventPool<ESyncEventMode::AutoReset>>().ReturnToPool(InEvent);
		}
	}
}

void HUniversalPlatformProcess::ClosePipe( void* ReadPipe, void* WritePipe )
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::ClosePipe not implemented on this platform");
}

bool HUniversalPlatformProcess::CreatePipe(void*& ReadPipe, void*& WritePipe, bool bWritePipeLocal)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::CreatePipe not implemented on this platform");

	return false;
}

bool HUniversalPlatformProcess::ReadPipeToVector(void* ReadPipe, std::vector<UInt8> & Output)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::ReadPipeToVector not implemented on this platform");

	return false;
}

bool HUniversalPlatformProcess::WritePipe(void* WritePipe, const UInt8* Data, const Int32 DataLength, Int32* OutDataLength)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::WritePipe not implemented on this platform");

	return false;
}

HUniversalPlatformProcess::HProcessSemaphore::HProcessSemaphore(const std::string& InName)
	:HProcessSemaphore(InName.c_str())
{

}

HUniversalPlatformProcess::HProcessSemaphore::HProcessSemaphore(const ANSICHAR* InName)
{
	strcpy_s(Name, sizeof(Name) -1, InName);
}

HUniversalPlatformProcess::HProcessSemaphore* 	HUniversalPlatformProcess::CreateProcessSynchSemaphore(const std::string& Name, bool bCreate, UInt32 MaxLocks)
{
	return CreateProcessSynchSemaphore(Name.c_str(), bCreate, MaxLocks);
}

HUniversalPlatformProcess::HProcessSemaphore*  HUniversalPlatformProcess::CreateProcessSynchSemaphore(const ANSICHAR* Name, bool bCreate, UInt32 MaxLocks)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::CreateProcessSynchSemaphore not implemented on this platform");
	return nullptr;
}

bool HUniversalPlatformProcess::DeleteProcessSynchSemaphore(HProcessSemaphore* InSemaphore)
{
	//TODO: Add log error message
	//K_LOG_FATAL(LOGTO_CONSOLE,"HUniversalPlatformProcess::DeleteProcessSynchSemaphore not implemented on this platform");
	return false;
}

