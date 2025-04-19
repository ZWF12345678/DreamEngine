#include "Windows/WindowsPlatformTLS.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"


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