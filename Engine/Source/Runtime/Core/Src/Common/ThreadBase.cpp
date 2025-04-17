#include "Common/ThreadBase.h"
#include "Template/TSingleton.h"
#include "Common/SyncEventPool.h"
#include "Common/PlatformProcess.h"


/*-----------------------------------------------------------------------------
								HThreadManager
-----------------------------------------------------------------------------*/
HThreadManager::HThreadManager(){}
HThreadManager::~HThreadManager()
{
	for (const std::pair<UInt32, HRunnableThread*>& Pair : ThreadsMap)
	{
		Pair.second->Kill();
	}
	ThreadsMap.clear();
}

bool HThreadManager::CheckThreadsMapSafeToContinueIteration()
{
	if (IsThreadsMapDirty)
	{
		//TODO: Add log error message
		//K_LOG_ERROR(LOGTO_CONSOLE,"HThreadManager::Threads was modified during unsafe iteration. Iteration will be aborted.");
		return false;
	}

	return true;
}

void HThreadManager::OnThreadsMapModified()
{
	IsThreadsMapDirty = true;
}

void HThreadManager::AddThread(UInt32 ThreadId, HRunnableThread* Thread)
{
	std::unique_lock<std::mutex> Lock(ThreadsMapMutex);
	if (!ThreadsMap.count(ThreadId))
	{
		ThreadsMap[ThreadId] = Thread;
		OnThreadsMapModified();
	}
}

void HThreadManager::RemoveThread(HRunnableThread* Thread)
{
	std::unique_lock<std::mutex> Lock(ThreadsMapMutex);
	if (ThreadsMap.count(Thread->GetThreadID()))
	{
		ThreadsMap.erase(Thread->GetThreadID());
		OnThreadsMapModified();
	}
}

//It's used when the Mode of thread is SingleThread
void HThreadManager::Tick()
{
	const bool bIsSingleThreadEnvironment = HPlatformProcess::SupportsMultithreading() == false;

	if (bIsSingleThreadEnvironment)
	{
		ForEachThread(
			[](UInt32 ThreadId, HRunnableThread* Thread)
			{
				if (Thread->GetThreadType() != HRunnableThread::ThreadType::Real)
				{
					Thread->Tick();
				}
			}
		);
	}
}

void HThreadManager::ForEachThread(std::function<void(UInt32 ThreadId, HRunnableThread* Thread)> Func)
{
	std::unique_lock<std::mutex> Lock(ThreadsMapMutex);
	HThreadsMap ThreadsCopy = ThreadsMap;
	for (const std::pair<UInt32, HRunnableThread*>& Pair : ThreadsCopy)
	{
		Func(Pair.first, Pair.second);
	}
}

const std::string& HThreadManager::GetThreadNameInternal(UInt32 ThreadId)
{
	static std::string NoThreadName;
	std::unique_lock<std::mutex> Lock(ThreadsMapMutex);
	auto Thraed = ThreadsMap.find(ThreadId);
	if (Thraed != ThreadsMap.end())
	{
		return Thraed->second->GetThreadName();
	}
	return NoThreadName;
}

HThreadManager& HThreadManager::Get()
{
	static HThreadManager Singleton;
	return Singleton;
}


/*-----------------------------------------------------------------------------
								HRunnableThread
-----------------------------------------------------------------------------*/

UInt32 HRunnableThread::RunnableTLSSlot = HRunnableThread::GetTLSSlot();

UInt32 HRunnableThread::GetTLSSlot()
{
    //TODO: Add log error message
	//KASSERT(IsInGameThread());
	UInt32 TlsSlot = HPlatformTLS::AllocateTLSSlot();
	//KASSERT(HPlatformTLS::IsValidTLSSlot(TlsSlot));
	return TlsSlot;

}

HRunnableThread::HRunnableThread()
	: Runnable(nullptr)
	, ThreadInitSyncEvent(nullptr)
	, ThreadAffinityMask(HPlatformAffinity::GetNoAffinityMask())
	, ThreadPriority(TPri_Normal)
	, ThreadID(0)
{
}

HRunnableThread::~HRunnableThread()
{
	/*if (!IsEngineExitRequested())
	{
		HThreadManager::Get().RemoveThread(this);
	}*/
	HThreadManager::Get().RemoveThread(this);
}

HRunnableThread* HRunnableThread::Create(
	class HRunnable* InRunnable,
	const ANSICHAR* ThreadName,
	UInt32 InStackSize,
	EThreadPriority InThreadPri,
	UInt64 InThreadAffinityMask,
	EThreadCreateFlags InCreateFlags)
{

	HRunnableThread* NewThread = nullptr;

	NewThread = HPlatformProcess::CreateRunnableThread();

	if (NewThread)
	{
		SetupCreatedThread(NewThread, InRunnable, ThreadName, InStackSize, InThreadPri, InThreadAffinityMask, InCreateFlags);
	}

	return NewThread;
}

void HRunnableThread::SetupCreatedThread(HRunnableThread*& NewThread, class HRunnable* InRunnable, const ANSICHAR* ThreadName, UInt32 InStackSize, EThreadPriority InThreadPri, UInt64 InThreadAffinityMask, EThreadCreateFlags InCreateFlags)
{
	bool bIsValid = NewThread->CreateInternal(InRunnable, ThreadName, InStackSize, InThreadPri, InThreadAffinityMask, InCreateFlags);

	if (bIsValid)
	{
		//TODO: Add log message
		//KASSERT(NewThread->Runnable);
	}
	else
	{
		delete NewThread;
		NewThread = nullptr;
	}
}

void HRunnableThread::SetTLS()
{
	//TODO: Add log error message
	//KASSERT(ThreadID == HPlatformTLS::GetCurrentThreadId());
	//KASSERT(HPlatformTLS::IsValidTLSSlot(RunnableTLSSlot));
	HPlatformTLS::SetTLSValue(RunnableTLSSlot, this);
}

void HRunnableThread::FreeTLS()
{
	//TODO: Add log error message
	//KASSERT(ThreadID == HPlatformTLS::GetCurrentThreadId());
	//KASSERT(HPlatformTLS::IsValidTLSSlot(RunnableTLSSlot));
	HPlatformTLS::SetTLSValue(RunnableTLSSlot, nullptr);
}


/*-----------------------------------------------------------------------------
								HSyncEventRef
-----------------------------------------------------------------------------*/
HSyncEventRef::HSyncEventRef(ESyncEventMode Mode)
{
	if(Mode == ESyncEventMode::AutoReset)
	{
		Event = Singleton::GetInstance<HSyncEventPool<ESyncEventMode::AutoReset>>().GetRawSyncEvent();
	}
	else
	{
		Event = Singleton::GetInstance<HSyncEventPool<ESyncEventMode::ManualReset>>().GetRawSyncEvent();
	}
}

HSyncEventRef::~HSyncEventRef()
{
	if (Event)
	{
		if (Event->IsManualReset())
		{
			Singleton::GetInstance<HSyncEventPool<ESyncEventMode::ManualReset>>().ReturnRawSyncEvent(Event);
		}
		else
		{
			Singleton::GetInstance<HSyncEventPool<ESyncEventMode::AutoReset>>().ReturnRawSyncEvent(Event);
		}
	}
}

/*-----------------------------------------------------------------------------
								HSharedSyncEventRef
-----------------------------------------------------------------------------*/

HSharedSyncEventRef::HSharedSyncEventRef(ESyncEventMode Mode)
	: Ptr(Singleton::GetInstance<HSyncEventPool<ESyncEventMode::AutoReset>>().GetRawSyncEvent(),
		[](HSyncEvent* Event) { Singleton::GetInstance<HSyncEventPool<ESyncEventMode::AutoReset>>().ReturnRawSyncEvent(Event); })
{
}
