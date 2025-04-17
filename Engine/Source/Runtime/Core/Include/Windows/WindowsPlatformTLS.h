#pragma once

#include "UniversalPlatform/UniversalPlatformTLS.h"

struct HWindowsPlatformTLS : public HUniversalPlatformTLS
{
    static UInt32 GetCurrentThreadID();

    static UInt32 AllocateTLSSlot();

    static void FreeTLSSlot(UInt32 SlotIndex);

    static void SetTLSValue(UInt32 SlotIndex, void* Value);

    static void* GetTLSValue(UInt32 SlotIndex);
};

typedef HWindowsPlatformTLS HPlatformTLS;