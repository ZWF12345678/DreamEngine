#include "Windows/WindowsSyncEvent.h"
#include "Windows/WindowsRunnableThread.h"
#include "Windows/WindowsPlatformProcess.h"

#include <Windows.h>

HRunnableThread* HWindowsPlatformProcess::CreateRunnableThread()
{
	return new HRunnableThreadWin();
}

UInt32 HWindowsPlatformProcess::GetCurrentProcessId()
{
	return ::GetCurrentProcessId();
}

UInt32 HWindowsPlatformProcess::GetCurrentCoreNumber()
{
	return ::GetCurrentProcessorNumber();
}                                                             

void HWindowsPlatformProcess::SetThreadAffinityMask(UInt64 InAffinityMask)
{
	::SetThreadAffinityMask(::GetCurrentThread(), InAffinityMask);
}

HSyncEvent* HWindowsPlatformProcess::CreateSynchEvent(bool bManualReset)
{
	bool isMultithread = HPlatformProcess::SupportsMultithreading();

	HSyncEvent* event = nullptr;

	if (isMultithread)
	{
		event = new HSyncEventWin();
	}

	if (event &&!event->Create(bManualReset))
	{
		delete event;
		event = nullptr;
	}
	return event;
}

/*-----------------------------------------------------------------------------
                        	WindowsSyncEvent
-----------------------------------------------------------------------------*/
HSyncEventWin::HSyncEventWin()
	:Event(nullptr)
	, ManualReset(false)
{
}
HSyncEventWin::~HSyncEventWin()
{
	if (Event)
	{
		::CloseHandle(Event);
	}
}

bool HSyncEventWin::Create(bool bManualReset)
{
	Event = CreateEvent(nullptr, bManualReset, false, nullptr);
	ManualReset = bManualReset;
	return Event != nullptr;
}

bool HSyncEventWin::IsManualReset() const
{
	return ManualReset;
}

void HSyncEventWin::Trigger()
{
	SetEvent(Event);
}

void HSyncEventWin::Reset()
{
	ResetEvent(Event);
}

bool HSyncEventWin::WaitFor(UInt32 WaitTime)
{
    //TODO
	//HOverSubscriptionScope _(WaitTime != 0); //Let Scehduler know we are waiting for something.
	return WaitForSingleObject(Event, WaitTime) == WAIT_OBJECT_0;
}

/*-----------------------------------------------------------------------------
                        	WindowsPlatformProcessImpl
-----------------------------------------------------------------------------*/

namespace WindowsPlatformProcessImpl
{
	static void SetThreadName(LPCSTR ThreadName)
	{
#if !PLATFORM_SEH_EXCEPTIONS_DISABLED
		const UInt32 MS_VC_EXCEPTION = 0x406D1388;

		struct THREADNAME_INFO
		{
			UInt32 dwType;
			LPCSTR szName;
			UInt32 dwThreadID;
			UInt32 dwFlags;
		};

		THREADNAME_INFO ThreadNameInfo;
		ThreadNameInfo.dwType = 0x1000;
		ThreadNameInfo.szName = ThreadName;
		ThreadNameInfo.dwThreadID = ::GetCurrentThreadId();
		ThreadNameInfo.dwFlags = 0;

		try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(ThreadNameInfo) / sizeof(ULONG_PTR), (ULONG_PTR*)&ThreadNameInfo);
		}
		catch (...)
		{
		}
#endif
	}

	static void SetThreadDescription(PCWSTR lpThreadDescription)
	{
		typedef HRESULT(WINAPI* SetThreadDescriptionFnPtr)(HANDLE hThread, PCWSTR lpThreadDescription);

#pragma warning( push )
#pragma warning( disable: 4191 )
		static SetThreadDescriptionFnPtr RealSetThreadDescription = (SetThreadDescriptionFnPtr)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "SetThreadDescription");
#pragma warning( pop )

		if (RealSetThreadDescription)
		{
			RealSetThreadDescription(::GetCurrentThread(), lpThreadDescription);
		}
	}
}

void HWindowsPlatformProcess::SetThreadName(const char* InName)
{
	WindowsPlatformProcessImpl::SetThreadDescription((const wchar_t*)InName);

	WindowsPlatformProcessImpl::SetThreadName(InName);
}

void HWindowsPlatformProcess::Sleep(float Seconds)
{
	UInt32 Milliseconds = (UInt32)(Seconds * 1000.0);
	if (Milliseconds == 0)
	{
		::SwitchToThread();
	}
	else
	{
		::Sleep(Milliseconds);
	}
}