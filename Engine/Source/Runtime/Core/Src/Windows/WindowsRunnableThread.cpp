#include "Common/Runnable.h"
#include "Common/SyscEvent.h"
#include "Common/ThreadManager.h"
#include "Common/PlatformProcess.h"
#include "Windows/WindowsRunnableThread.h"

#include<windows.h>


::DWORD STDCALL HRunnableThreadWin::_ThreadProc(LPVOID lpParam)
{   
    //TODO: Add log error message
    //KASSERT(pThis);
    auto* ThisThread = (HRunnableThreadWin*)lpParam;
    HThreadManager::Get().AddThread(ThisThread->GetThreadID(), ThisThread);
    return ThisThread->GuardedRun();
}

UInt32 HRunnableThreadWin::Run()
{
    UInt32 ExitCode = -1;

    if (Runnable->Init() == true)
    {
        ThreadInitSyncEvent->Trigger();

        SetTLS();

        ExitCode = Runnable->Run();

        Runnable->Exit();

        FreeTLS();
    }
    else
    {
        ThreadInitSyncEvent->Trigger();
    }
    return ExitCode;
}

UInt32 HRunnableThreadWin::GuardedRun()
{
    UInt32 ExitCode = -1;

    /*HPlatformProcess::SetThreadAffinityMask(ThreadAffinityMask);*/
    HPlatformProcess::SetThreadName(ThreadName.c_str());
    {
        ExitCode = Run();
    }
    return ExitCode;
}


HRunnableThreadWin::~HRunnableThreadWin()
{
    if (ThreadHandle)
    {
        //TODO: Add log error message
	    //K_LOG_DEBUG(LOGTO_CONSOLE, "Thread {0} killed\n", ThreadName.c_str());
	    Kill(true);
    }
}

void HRunnableThreadWin::SetThreadPriority(EThreadPriority NewPriority)
{
    ThreadPriority = NewPriority;

    ::SetThreadPriority(ThreadHandle, TranslateThreadPriority(ThreadPriority));
}

void HRunnableThreadWin::Suspend(bool bShouldPause)
{
    //TODO: Add log error message
    //KASSERT(Thread);
    if (bShouldPause == true)
    {
	    SuspendThread(ThreadHandle);
    }
    else
    {
        ResumeThread(ThreadHandle);
    }
}

bool HRunnableThreadWin::Kill(bool bShouldWait)
{
    //TODO: Add log error message
    //KASSERT_MSG(Thread, "Thread is not valid");
    bool bDidExitOK = true;

    if (Runnable)
    {
        Runnable->Stop();
    }

    if (bShouldWait == true)
    {
        WaitForSingleObject(ThreadHandle, INFINITE);
    }

    CloseHandle(ThreadHandle);
    ThreadHandle = NULL;
    return bDidExitOK;
}

void HRunnableThreadWin::WaitForCompletion()
{
    WaitForSingleObject(ThreadHandle, INFINITE);
}

int HRunnableThreadWin::TranslateThreadPriority(EThreadPriority Priority)
{
	switch (Priority)
	{
	case TPri_AboveNormal:				return THREAD_PRIORITY_ABOVE_NORMAL;
	case TPri_Normal:					return THREAD_PRIORITY_NORMAL;
	case TPri_BelowNormal:				return THREAD_PRIORITY_BELOW_NORMAL;
	case TPri_Highest:					return THREAD_PRIORITY_HIGHEST;
	case TPri_TimeCritical:				return THREAD_PRIORITY_HIGHEST;
	case TPri_Lowest:					return THREAD_PRIORITY_LOWEST;
	case TPri_SlightlyBelowNormal:	    return THREAD_PRIORITY_NORMAL;
	default:							return THREAD_PRIORITY_NORMAL;
	}
}

bool HRunnableThreadWin::CreateInternal(HRunnable* InRunnable, const ANSICHAR* InThreadName,
    UInt32 InStackSize,
    EThreadPriority InThreadPri, UInt64 InThreadAffinityMask,
    EThreadCreateFlags InCreateFlags)
{
    static bool bOnce = false;
    if (!bOnce)
    {
        bOnce = true;
        ::SetThreadPriority(::GetCurrentThread(), TranslateThreadPriority(TPri_Normal));
    }
    //TODO: Add log error message
    //KASSERT(InRunnable);
    Runnable = InRunnable;
    ThreadAffinityMask = InThreadAffinityMask;

    ThreadInitSyncEvent = HPlatformProcess::GetSynchEventFromPool(true);

    ThreadName = InThreadName ? InThreadName : "Unnamed Thread";
    ThreadPriority = InThreadPri;

    ThreadHandle = CreateThread(NULL, InStackSize, _ThreadProc, this, STACK_SIZE_PARAM_IS_A_RESERVATION | CREATE_SUSPENDED, (::DWORD*)&ThreadID);

    if (ThreadHandle == NULL)
    {
        Runnable = nullptr;
    }
    else
    {
        ResumeThread(ThreadHandle);

        ThreadInitSyncEvent->WaitFor(INFINITE);

        ThreadPriority = TPri_Normal;

        SetThreadPriority(InThreadPri);
    }

    HPlatformProcess::ReturnSynchEventToPool(ThreadInitSyncEvent);
    ThreadInitSyncEvent = nullptr;
    return ThreadHandle != NULL;
}