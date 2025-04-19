#pragma once

#include "UniversalPlatform/UniversalPlatformMisc.h"

struct HWindowsPlatformMisc : public HUniversalPlatformMisc
{
    static CORE_API const ANSICHAR* GetSystemErrorMessage(ANSICHAR* OutBuffer, Int32 BufferCount, Int32 Error);

    static CORE_API bool HasTimedPauseCPUFeature();
};

typedef HWindowsPlatformMisc HPlatformMisc;