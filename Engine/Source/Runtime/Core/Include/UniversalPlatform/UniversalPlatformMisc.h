#pragma once

#include "CoreType.h"

struct HUniversalPlatformMisc
{
    static CORE_API const ANSICHAR* GetSystemErrorMessage(ANSICHAR* OutBuffer, Int32 BufferCount, Int32 Error);
};