#include "Template/TSingleton.h"
#include "Common/SyncEventPool.h"
#include "UniversalPlatform/UniversalPlatformProcess.h"

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
	//K_LOG_FATAL(LOGTO_CONSOLE,"FGenericPlatformProcess::CreateThread not implemented on this platform");
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