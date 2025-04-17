#pragma once

#include "Common/RunnableThread.h"


typedef void*               HANDLE;
typedef void*               LPVOID;
typedef unsigned long       DWORD;

class HRunnableThreadWin : public HRunnableThread
{
    HANDLE ThreadHandle = 0;

    static ::DWORD STDCALL _ThreadProc(LPVOID lpParam);

    UInt32 Run();

    UInt32 GuardedRun();

public:
    ~HRunnableThreadWin();

    virtual void SetThreadPriority(EThreadPriority NewPriority) override;

    virtual void Suspend(bool bShouldPause = true) override;

    virtual bool Kill(bool bShouldWait = true) override;

    virtual void WaitForCompletion() override;

    static int TranslateThreadPriority(EThreadPriority Priority);

protected:

virtual bool CreateInternal(HRunnable* InRunnable, const ANSICHAR* InThreadName,
	UInt32 InStackSize = 0,
	EThreadPriority InThreadPri = TPri_Normal, UInt64 InThreadAffinityMask = 0,
	EThreadCreateFlags InCreateFlags = EThreadCreateFlags::None) override;

};
