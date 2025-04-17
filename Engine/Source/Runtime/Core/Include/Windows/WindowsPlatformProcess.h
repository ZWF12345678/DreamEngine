#pragma once

#include "UniversalPlatform/UniversalPlatformProcess.h"

struct HWindowsPlatformProcess : public HUniversalPlatformProcess
{
    static CORE_API class HRunnableThread* CreateRunnableThread();

    static CORE_API UInt32 GetCurrentProcessId();

    static CORE_API UInt32 GetCurrentCoreNumber();

    static CORE_API void SetThreadAffinityMask(UInt64 InAffinityMask);

    static void SetThreadName(const char* InName);

    static CORE_API HSyncEvent* CreateSynchEvent(bool IsManualReset);

    static CORE_API void Sleep(float Seconds);
};

typedef HWindowsPlatformProcess HPlatformProcess;