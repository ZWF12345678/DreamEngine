#pragma once

#include "UniversalPlatform/UniversalPlatformMisc.h"

struct HWindowsPlatformMisc : public HUniversalPlatformMisc
{
    static CORE_API bool HasTimedPauseCPUFeature();
};

typedef HWindowsPlatformMisc HPlatformMisc;