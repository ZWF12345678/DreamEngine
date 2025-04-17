#pragma once

#include "CoreType.h"

#include COMPILED_PLATFORM_HEADER(PlatformAffinity.h)

struct SThreadAffinity
{
	UInt64 ThreadAffinityMask = HPlatformAffinity::GetNoAffinityMask();
	UInt16 ProcessorGroup = 0;
};