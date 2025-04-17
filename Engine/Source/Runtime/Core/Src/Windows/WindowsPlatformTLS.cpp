#include "Windows/WindowsPlatformTLS.h"

#include <processthreadsapi.h>

UInt32 HWindowsPlatformTLS::GetCurrentThreadID()
{
    return ::GetCurrentThreadId();
}

UInt32 HWindowsPlatformTLS::AllocateTLSSlot()
{
    return ::TlsAlloc();
}

void HWindowsPlatformTLS::FreeTLSSlot(UInt32 SlotIndex)
{
    ::TlsFree(SlotIndex);
}

void HWindowsPlatformTLS::SetTLSValue(UInt32 SlotIndex, void* Value)
{
    ::TlsSetValue(SlotIndex, Value);
}

void* HWindowsPlatformTLS::GetTLSValue(UInt32 SlotIndex)
{
    return ::TlsGetValue(SlotIndex);
}