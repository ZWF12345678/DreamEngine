#include "Windows/WindowsSyncEvent.h"
#include "Windows/WindowsPlatformMisc.h"
#include "Windows/WindowsRunnableThread.h"
#include "Windows/WindowsPlatformProcess.h"


#include <set>
#include <Windows.h>
#include <psapi.h>
#include <TlHelp32.h>


bool IsJobObjectSet = false;
HANDLE GhJob = NULL;

void HWindowsPlatformProcess::HWindowsProcessSemaphore::Lock()
{
	DWORD WaitResult = WaitForSingleObject(Semaphore, INFINITE);

	if (WaitResult != WAIT_OBJECT_0)
	{
		//TODO: Handle error
		DWORD Error = GetLastError();
	}
}

bool HWindowsPlatformProcess::HWindowsProcessSemaphore::TryLock(UInt64 NanosecondsToWait)
{
	DWORD MillisecondsToWait = NanosecondsToWait / 1000000ULL;
	DWORD WaitResult = WaitForSingleObject(Semaphore, MillisecondsToWait);

	if (WaitResult != WAIT_OBJECT_0 && WaitResult != WAIT_TIMEOUT)	// timeout is not a warning
	{
		DWORD Error = GetLastError();
	}

	return WaitResult == WAIT_OBJECT_0;
}

void HWindowsPlatformProcess::HWindowsProcessSemaphore::Unlock()
{
	if (!ReleaseSemaphore(Semaphore, 1, NULL))
	{
		DWORD Error = GetLastError();
	}
}

HWindowsPlatformProcess::HWindowsProcessSemaphore::HWindowsProcessSemaphore(const std::string& InName, HANDLE InSemaphore)
	: HWindowsProcessSemaphore(InName.c_str(), InSemaphore)
{
}

HWindowsPlatformProcess::HWindowsProcessSemaphore::HWindowsProcessSemaphore(const ANSICHAR* InName, HANDLE InSemaphore)
	: HProcessSemaphore(InName)
	, Semaphore(InSemaphore)
{
}

HWindowsPlatformProcess::HWindowsProcessSemaphore::~HWindowsProcessSemaphore()
{
}


HWindowsPlatformProcess::HProcessSemaphore* HWindowsPlatformProcess::CreateProcessSynchSemaphore(const std::string& Name, bool bCreate, UInt32 MaxLocks)
{
	return CreateProcessSynchSemaphore(Name.c_str(), bCreate, MaxLocks);
}

HWindowsPlatformProcess::HProcessSemaphore* HWindowsPlatformProcess::CreateProcessSynchSemaphore(const ANSICHAR* Name, bool bCreate, UInt32 MaxLocks)
{
	HANDLE Semaphore = NULL;
	
	if (bCreate)
	{
		Semaphore = CreateSemaphore(NULL, MaxLocks, MaxLocks, Name);
		if (NULL == Semaphore)
		{
			DWORD ErrNo = GetLastError();
		}
	}
	else
	{
		DWORD AccessRights = SYNCHRONIZE | SEMAPHORE_MODIFY_STATE;
		Semaphore = OpenSemaphore(AccessRights, false, Name);
		if (Semaphore == NULL)
		{
			DWORD ErrNo = GetLastError();
		}
	}

	return new HWindowsProcessSemaphore(Name, Semaphore);
	
}

SProcessHandle HWindowsPlatformProcess::OpenProcess(UInt32 ProcessID)
{
	HANDLE ProcessHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
	return SProcessHandle(ProcessHandle);
}

void HWindowsPlatformProcess::CloseProcess( SProcessHandle & ProcessHandle )
{
	if (ProcessHandle.IsValid())
	{
		::CloseHandle(ProcessHandle.Get());
		ProcessHandle.Reset();
	}
}

bool HWindowsPlatformProcess::ProcessIsRunning( SProcessHandle & ProcessHandle )
{
	bool ApplicationRunning = true;
	UInt32 WaitResult = ::WaitForSingleObject(ProcessHandle.Get(), 0);
	if (WaitResult != WAIT_TIMEOUT)
	{
		ApplicationRunning = false;
	}
	return ApplicationRunning;
}

void HWindowsPlatformProcess::WaitForProcess( SProcessHandle & ProcessHandle )
{
	::WaitForSingleObject(ProcessHandle.Get(), INFINITE);
}

void HWindowsPlatformProcess::TerminateProcess( SProcessHandle & ProcessHandle, bool KillTree)
{
	if (KillTree)
	{
		TerminateProcessTreeWithPredicate(ProcessHandle, [](UInt32 ProcessId, const TCHAR* ApplicationName) { return true; });
	}
	else
	{
		::TerminateProcess(ProcessHandle.Get(),0);
	}
}

static void TerminateProcessTreeWithPredicateInternal(
	HANDLE ProcessHandle,
	std::function<bool(UInt32 ProcessId, const ANSICHAR* ApplicationName)> Predicate,
	std::set<DWORD>& VisitedProcessIds)
{
	::DWORD ProcessId = ::GetProcessId(ProcessHandle);
	std::string ProcessName = HPlatformProcess::GetApplicationName(ProcessId);

	VisitedProcessIds.insert(ProcessId);

	if (!Predicate(ProcessId, ProcessName.c_str()))
	{
		return;
	}

	HANDLE SnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (SnapShot != INVALID_HANDLE_VALUE)
	{

		PROCESSENTRY32 Entry;
		Entry.dwSize = sizeof(PROCESSENTRY32);

		if (::Process32First(SnapShot, &Entry))
		{
			do
			{
				if (Entry.th32ParentProcessID == ProcessId)
				{
					HANDLE ChildProcHandle = ::OpenProcess(PROCESS_ALL_ACCESS, 0, Entry.th32ProcessID);

					if (ChildProcHandle)
					{
						if (!VisitedProcessIds.count(Entry.th32ProcessID))
						{
							TerminateProcessTreeWithPredicateInternal(ChildProcHandle, Predicate, VisitedProcessIds);
						}
						::CloseHandle(ChildProcHandle);
					}
				}
			}
			while(::Process32Next(SnapShot, &Entry));
		}
	}
	::CloseHandle(SnapShot);

	TerminateProcess(ProcessHandle,0);
}

void HWindowsPlatformProcess::TerminateProcessTreeWithPredicate(SProcessHandle& ProcessHandle,std::function<bool(UInt32 ProcessId, const ANSICHAR* ApplicationName)> Predicate)
{
	std::set<DWORD> VisitedProcessIds;

	TerminateProcessTreeWithPredicateInternal
	(
		ProcessHandle.Get(),
		Predicate,
		VisitedProcessIds
	);
}

bool HWindowsPlatformProcess::GetProcessReturnCode( SProcessHandle& ProcHandle, Int32& ReturnCode )
{
	DWORD ExitCode = 0;
	if (::GetExitCodeProcess(ProcHandle.Get(), &ExitCode) && ExitCode != STILL_ACTIVE)
	{
		if (ReturnCode)
		{
			ReturnCode = (Int32)ExitCode;
		}
		return true;
	}
	return false;
}

bool HWindowsPlatformProcess::ApplicationIsRunning( UInt32 ProcessId )
{
	bool ApplicationRunning = true;
	HANDLE ProcessHandle = ::OpenProcess(SYNCHRONIZE, false, ProcessId);
	if (ProcessHandle == NULL)
	{
		ApplicationRunning = false;
	}
	else
	{
		UInt32 WaitResult = WaitForSingleObject(ProcessHandle, 0);
		if (WaitResult != WAIT_TIMEOUT)
		{
			ApplicationRunning = false;
		}
		::CloseHandle(ProcessHandle);
	}
	return ApplicationRunning;
}

std::string HWindowsPlatformProcess::GetApplicationName( UInt32 ProcessId )
{
	std::string Output{""};
	HANDLE ProcessHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, ProcessId);
	if (ProcessHandle != NULL)
	{
		const Int32 ProcessNameBufferSize = 4096;
		ANSICHAR ProcessNameBuffer[ProcessNameBufferSize];
		
		Int32 InOutSize = ProcessNameBufferSize;
		static_assert(sizeof(::DWORD) == sizeof(Int32), "DWORD size doesn't match Int32. Is it the future or the past?");

		if (QueryFullProcessImageName(ProcessHandle, 0, ProcessNameBuffer, (PDWORD)(&InOutSize)))
		{
			Output = ProcessNameBuffer;
		}

		::CloseHandle(ProcessHandle);
	}
	return Output;
}

bool HWindowsPlatformProcess::GetApplicationMemoryUsed(UInt32 ProcessId, SIZE_T& OutMemoryUsedSize)
{
	bool bSuccess = false;
	HANDLE ProcessHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, ProcessId);

	if (ProcessHandle != NULL)
	{
		PROCESS_MEMORY_COUNTERS_EX MemoryInfo;

		if (GetProcessMemoryInfo(ProcessHandle, (PROCESS_MEMORY_COUNTERS*)&MemoryInfo, sizeof(MemoryInfo)))
		{
			OutMemoryUsedSize = MemoryInfo.PrivateUsage;
			bSuccess = true;
		}

		::CloseHandle(ProcessHandle);
	}
	return bSuccess;
}

bool HWindowsPlatformProcess::ExecProcess(const ANSICHAR* URL, const ANSICHAR* Params, Int32& OutReturnCode, std::string& OutStdOut, std::string& OutStdErr, const ANSICHAR* OptionalWorkingDirectory , bool bShouldEndWithParentProcess)
{
	if (bShouldEndWithParentProcess && !IsJobObjectSet)
	{
		GhJob = CreateJobObject(NULL, NULL);
		if (!GhJob)
		{
			//TODO: Handle error
		}
		else
		{
			JOBOBJECT_EXTENDED_LIMIT_INFORMATION LimitInformation;
			ZeroMemory(&LimitInformation, sizeof(LimitInformation));
			LimitInformation.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
			if (0 == SetInformationJobObject(GhJob, JobObjectExtendedLimitInformation, &LimitInformation, sizeof(LimitInformation)))
			{
				//TODO: Handle error
			}
		}
		IsJobObjectSet = true;
	}

	STARTUPINFOEX StartupInfoEx;
	ZeroMemory(&StartupInfoEx, sizeof(StartupInfoEx));
	StartupInfoEx.StartupInfo.cb = sizeof(StartupInfoEx);
	StartupInfoEx.StartupInfo.dwX = CW_USEDEFAULT;
	StartupInfoEx.StartupInfo.dwY = CW_USEDEFAULT;
	StartupInfoEx.StartupInfo.dwXSize = CW_USEDEFAULT;
	StartupInfoEx.StartupInfo.dwYSize = CW_USEDEFAULT;
	StartupInfoEx.StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfoEx.StartupInfo.wShowWindow = SW_SHOWMINNOACTIVE;
	StartupInfoEx.StartupInfo.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);

	HANDLE hStdOutRead = NULL;
	HANDLE hStdErrRead = NULL;

	std::vector<UInt8> AttributeList;


	StartupInfoEx.StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
	SECURITY_ATTRIBUTES Attr;
	ZeroMemory(&Attr, sizeof(Attr));
	Attr.nLength = sizeof(SECURITY_ATTRIBUTES);
	Attr.bInheritHandle = TRUE;
	bool bCreateStdOut = ::CreatePipe(&hStdOutRead, &StartupInfoEx.StartupInfo.hStdOutput, &Attr, 0);
	bool bCreateStdErr = ::CreatePipe(&hStdErrRead, &StartupInfoEx.StartupInfo.hStdError, &Attr, 0);
	SIZE_T BufferSize = 0;
	if(!InitializeProcThreadAttributeList(NULL, 1, 0, &BufferSize) && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		AttributeList.resize(BufferSize);
		StartupInfoEx.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)AttributeList.data();
		bool bInit = InitializeProcThreadAttributeList(StartupInfoEx.lpAttributeList, 1, 0, &BufferSize);
	}
	HANDLE InheritHandles[2] = { StartupInfoEx.StartupInfo.hStdOutput, StartupInfoEx.StartupInfo.hStdError };
	bool bUpdate = UpdateProcThreadAttribute(StartupInfoEx.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, InheritHandles, sizeof(InheritHandles), NULL, NULL);

	bool bSuccess = false;

	std::string CommandLine;

	size_t CmdBufferSize = strlen(URL) + strlen(Params) + 4; // 4 accounts for quotes, space, and null terminator
	std::vector<char> Buffer(BufferSize);

	if (URL[0] != '\"') // Don't quote executable name if it's already quoted
	{
		snprintf(Buffer.data(), Buffer.size(), "\"%s\" %s", URL, Params);
	}
	else
	{
		snprintf(Buffer.data(), Buffer.size(), "%s %s", URL, Params);
	}

	CommandLine = Buffer.data();

	UInt32 CreateFlags = NORMAL_PRIORITY_CLASS | DETACHED_PROCESS;
	if (StartupInfoEx.lpAttributeList != NULL)
	{
		CreateFlags |= EXTENDED_STARTUPINFO_PRESENT;
	}

	PROCESS_INFORMATION ProcInfo;

	if (CreateProcess(NULL, CommandLine.data(), NULL, NULL, TRUE, CreateFlags, NULL, OptionalWorkingDirectory, &StartupInfoEx.StartupInfo, &ProcInfo))
	{
		if (bShouldEndWithParentProcess && GhJob)
		{
			int RetVal = AssignProcessToJobObject(GhJob, ProcInfo.hProcess);
			if (RetVal == 0)
			{
				//TODO: Handle error:AssignProcessToObject failed.
			}
		}
		if (hStdOutRead != NULL)
		{
			HANDLE ReadablePipes[2] = { hStdOutRead, hStdErrRead };
			std::string* OutStrings[2] = { &OutStdOut, &OutStdErr };
			std::vector<UInt8> PipeBytes[2];

			auto ReadPipes = [&]()
			{
				for (Int32 PipeIndex = 0; PipeIndex < 2; ++PipeIndex)
				{
					if (ReadablePipes[PipeIndex] && OutStrings[PipeIndex])
					{
						std::vector<UInt8> BinaryData;
						ReadPipeToVector(ReadablePipes[PipeIndex], BinaryData);
						PipeBytes[PipeIndex].insert(PipeBytes[PipeIndex].end(), BinaryData.begin(), BinaryData.end());
					}
				}
			};

			SProcessHandle ProcHandle(ProcInfo.hProcess);
			do
			{
				ReadPipes();
				HPlatformProcess::Sleep(0);
			} while (ProcessIsRunning(ProcHandle));
			ReadPipes();

			// Convert only after all bytes are available to prevent string corruption
			for (Int32 PipeIndex = 0; PipeIndex < 2; ++PipeIndex)
			{
				if (OutStrings[PipeIndex] && PipeBytes[PipeIndex].size() > 0)
				{
					PipeBytes[PipeIndex].push_back('\0');
					*OutStrings[PipeIndex] = (const ANSICHAR*)PipeBytes[PipeIndex].data();
				}
			}
		}
		else
		{
			::WaitForSingleObject(ProcInfo.hProcess, INFINITE);
		}
		if (OutReturnCode)
		{
			bool ExitCode = ::GetExitCodeProcess(ProcInfo.hProcess, (DWORD*)&OutReturnCode);
			//TODO: Handle error: GetExitCodeProcess failed.
		}
		::CloseHandle(ProcInfo.hProcess);
		::CloseHandle(ProcInfo.hThread);
		bSuccess = true;
	}
	else
	{
		DWORD ErrorCode = GetLastError();

		// if CreateProcess failed, we should return a useful error code, which GetLastError will have
		if (OutReturnCode)
		{
			OutReturnCode = ErrorCode;
		}

		ANSICHAR ErrorMessage[512];

		HWindowsPlatformMisc::GetSystemErrorMessage(ErrorMessage, 512, ErrorCode);

		//TODO: Handle error: CreateProcess failed:MSG:"CreateProc failed: %s (0x%08x)"), ErrorMessage, ErrorCode
		if (ErrorCode == ERROR_NOT_ENOUGH_MEMORY || ErrorCode == ERROR_OUTOFMEMORY)
		{
			// These errors are common enough that we want some available memory information
			//HPlatformMemoryStats Stats = HPlatformMemory::GetStats();
			//TODO: "Mem used: %.2f MB, OS Free %.2f MB", Stats.UsedPhysical / 1048576.0f, Stats.AvailablePhysical / 1048576.0f
		}
		//TODO:"URL: %s %s", URL, Params
	}

	if(StartupInfoEx.StartupInfo.hStdOutput != NULL)
	{
		CloseHandle(StartupInfoEx.StartupInfo.hStdOutput);
	}
	if(StartupInfoEx.StartupInfo.hStdError != NULL)
	{
		CloseHandle(StartupInfoEx.StartupInfo.hStdError);
	}
	if(hStdOutRead != NULL)
	{
		CloseHandle(hStdOutRead);
	}
	if(hStdErrRead != NULL)
	{
		CloseHandle(hStdErrRead);
	}

	if(StartupInfoEx.lpAttributeList != NULL)
	{
		DeleteProcThreadAttributeList(StartupInfoEx.lpAttributeList);
	}

	return bSuccess;
}

bool HWindowsPlatformProcess::ExecAdminProcess(const ANSICHAR* URL, const ANSICHAR* Params, Int32& OutReturnCode)
{
	

	return false;
}

/*
 * Deletes an interprocess synchronization object.
 */
bool HWindowsPlatformProcess::DeleteProcessSynchSemaphore(HProcessSemaphore* InSemaphore)
{
	if (InSemaphore == nullptr)
	{
		return false;
	}

	HWindowsProcessSemaphore * WinProcessSemaphore = static_cast< HWindowsProcessSemaphore * >(InSemaphore);

	HANDLE Semaphore = WinProcessSemaphore->GetSemaphore();
	bool bSucceeded = false;
	if (Semaphore)
	{
		bSucceeded = (CloseHandle(Semaphore) == TRUE);
		if (!bSucceeded)
		{
			DWORD ErrNo = GetLastError();
		}
	}

	// delete anyways
	delete InSemaphore;

	return bSucceeded;
}


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

void HWindowsPlatformProcess::ClosePipe(void* ReadPipe, void* WritePipe)
{
	if (ReadPipe != nullptr && ReadPipe != INVALID_HANDLE_VALUE )
	{
		::CloseHandle((HANDLE)ReadPipe);
	}
	if (WritePipe != nullptr && WritePipe != INVALID_HANDLE_VALUE)
	{
		::CloseHandle((HANDLE)WritePipe);
	}
}

bool HWindowsPlatformProcess::CreatePipe(void*& ReadPipe, void*& WritePipe, bool bWritePipeLocal)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.lpSecurityDescriptor = nullptr;
	saAttr.bInheritHandle = true;

	if (!::CreatePipe(&ReadPipe, &WritePipe, &saAttr, 0))
	{
		return false;
	}

	if (!::SetHandleInformation(bWritePipeLocal ? WritePipe : ReadPipe, HANDLE_FLAG_INHERIT, 0))
	{
		return false;
	}

	return true;
}

bool HWindowsPlatformProcess::ReadPipeToVector(void* ReadPipe, std::vector<UInt8> & Output)
{
	UInt32 BytesAvailable = 0;
	if (::PeekNamedPipe(ReadPipe, NULL, 0, NULL, (::DWORD*)&BytesAvailable, NULL) && (BytesAvailable > 0))
	{
		Output.resize(BytesAvailable);
		UInt32 BytesRead = 0;
		if (::ReadFile(ReadPipe, Output.data(), BytesAvailable, (::DWORD*)&BytesRead, NULL))
		{
			if (BytesRead < BytesAvailable)
			{
				Output.resize(BytesRead);
			}

			return true;
		}
		else
		{
			Output.clear();
		}
	}

	return false;
}

bool HWindowsPlatformProcess::WritePipe(void* WritePipe, const UInt8* Data, const Int32 DataLength, Int32* OutDataLength)
{
	// if there is not a message or WritePipe is null
	if ((DataLength == 0) || (WritePipe == nullptr))
	{
		return false;
	}

	// write to pipe
	UInt32 BytesWritten = 0;
	bool bIsWritten = !!WriteFile(WritePipe, Data, DataLength, (::DWORD*)&BytesWritten, nullptr);

	// Get written Data Length
	if (OutDataLength)
	{
		*OutDataLength = (Int32)BytesWritten;
	}

	return bIsWritten;
}

void HWindowsPlatformProcess::Sleep(float Seconds)
{
	SleepNoStats(Seconds);
}

void HWindowsPlatformProcess::SleepNoStats( float Seconds )
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

void HWindowsPlatformProcess::SleepInfinite()
{
	while (true)
	{
		::Sleep(INFINITE);
	}
}

void HWindowsPlatformProcess::YieldThread()
{
	::SwitchToThread();
}