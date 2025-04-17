#include "Misc/TimeSpan.h"
#include "Windows/WindowsPlatformTime.h"

#include <windows.h>

float HWindowsPlatformTime::CPUTimePctRelative = 0.0f;

namespace HWindowsTimeInternal
{
	struct HThreadCPUStats
	{
        /** Current thread percentage CPU utilization for the last interval relative to one core. */
		float ThreadCPUTimePctRelative = 0.f;

        /** The per-thread CPU processing time (kernel + user) from the last update */
		double LastIntervalThreadTime = 0;
	};

	thread_local HThreadCPUStats CurrentThreadCPUStats = {};
}

//double HWindowsPlatformTime::InitTiming(void)
//{
//	LARGE_INTEGER Frequency;
//	SecondsPerCycle = 1.0 / (double)Frequency.QuadPart;
//	SecondsPerCycle64 = 1.0 / (double)Frequency.QuadPart;
//
//	static const float PollingInterval = 1.0f / 4.0f;
//
//	// Register a ticker delegate for updating the CPU utilization data.
//	HTSTicker::GetCoreTicker().AddTicker(HTickerDelegate::CreateStatic(&HPlatformTime::UpdateCPUTime), PollingInterval);
//
//	return HPlatformTime::Seconds();
//}

double HWindowsPlatformTime::Seconds()
{
    ::LARGE_INTEGER Cycles;
    ::QueryPerformanceCounter(&Cycles);

    // add big number to make bugs apparent where return value is being passed to float
    return (double)Cycles.QuadPart * GetSecondsPerCycle() + 16777216.0;
}

UInt32 HWindowsPlatformTime::Cycles()
{
    ::LARGE_INTEGER Cycles;
	::QueryPerformanceCounter(&Cycles);
	return (UInt32)Cycles.QuadPart;
}

UInt64 HWindowsPlatformTime::Cycles64()
{
    ::LARGE_INTEGER Cycles;
	::QueryPerformanceCounter(&Cycles);
	return Cycles.QuadPart;
}

void HWindowsPlatformTime::SystemTime(Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	Year = st.wYear;
	Month = st.wMonth;
	DayOfWeek = st.wDayOfWeek;
	Day = st.wDay;
	Hour = st.wHour;
	Min = st.wMinute;
	Sec = st.wSecond;
	MSec = st.wMilliseconds;
}

void HWindowsPlatformTime::UtcTime(Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	Year = st.wYear;
	Month = st.wMonth;
	DayOfWeek = st.wDayOfWeek;
	Day = st.wDay;
	Hour = st.wHour;
	Min = st.wMinute;
	Sec = st.wSecond;
	MSec = st.wMilliseconds;
}

/** Holds Windows filetime misc functions. */
struct HFiletimeMisc
{
    /**
	 * @return number of ticks based on the specified Filetime.
	 */
	static FORCEINLINE UInt64 TicksFromFileTime(const FILETIME& Filetime)
	{
		const UInt64 NumTicks = (UInt64(Filetime.dwHighDateTime) << 32) + Filetime.dwLowDateTime;
		return NumTicks;
	}
    
    /**
	 * @return number of seconds based on the specified Filetime.
	 */
	static FORCEINLINE double ToSeconds(const FILETIME& Filetime)
	{
		return double(TicksFromFileTime(Filetime)) / double(ETimespan::TicksPerSecond);
	}
};

bool HWindowsPlatformTime::UpdateCPUTime(float /*DeltaTime*/)
{

	static double LastTotalProcessTime = 0.0f;
	static double LastTotalUserAndKernelTime = 0.0f;

	FILETIME CreationTime = { 0 };
	FILETIME ExitTime = { 0 };
	FILETIME KernelTime = { 0 };
	FILETIME UserTime = { 0 };
	FILETIME CurrentTime = { 0 };

	::GetProcessTimes(::GetCurrentProcess(), &CreationTime, &ExitTime, &KernelTime, &UserTime);
	::GetSystemTimeAsFileTime(&CurrentTime);

	const double CurrentTotalUserAndKernelTime = HFiletimeMisc::ToSeconds(KernelTime) + HFiletimeMisc::ToSeconds(UserTime);
	const double CurrentTotalProcessTime = HFiletimeMisc::ToSeconds(CurrentTime) - HFiletimeMisc::ToSeconds(CreationTime);

	const double IntervalProcessTime = CurrentTotalProcessTime - LastTotalProcessTime;
	const double IntervalUserAndKernelTime = CurrentTotalUserAndKernelTime - LastTotalUserAndKernelTime;

	// IntervalUserAndKernelTime == 0.0f means that the OS hasn't updated the data yet, 
	// so don't update to avoid oscillating between 0 and calculated value.
	if (IntervalUserAndKernelTime > 0.0)
	{
		CPUTimePctRelative = (float)(IntervalUserAndKernelTime / IntervalProcessTime * 100.0);

		LastTotalProcessTime = CurrentTotalProcessTime;
		LastTotalUserAndKernelTime = CurrentTotalUserAndKernelTime;
		LastIntervalCPUTimeInSeconds = IntervalUserAndKernelTime;
	}

	return true;
}

bool HWindowsPlatformTime::UpdateThreadCPUTime(float/*= 0.0*/)
{
	struct FThreadCPUTime
	{
		double LastTotalThreadTime = 0.0;
		double LastTotalThreadUserAndKernelTime = 0.0;
	};

	thread_local FThreadCPUTime ThreadTimeInfo = {};

	FILETIME ThreadCreationTime = { 0 };
	FILETIME ThreadExitTime = { 0 };
	FILETIME ThreadKernelTime = { 0 };
	FILETIME ThreadUserTime = { 0 };
	FILETIME CurrentTime = { 0 };

	::GetThreadTimes(::GetCurrentThread(), &ThreadCreationTime, &ThreadExitTime, &ThreadKernelTime, &ThreadUserTime);
	::GetSystemTimeAsFileTime(&CurrentTime);

	const double CurrentTotalThreadUserAndKernelTime = HFiletimeMisc::ToSeconds(ThreadKernelTime) + HFiletimeMisc::ToSeconds(ThreadUserTime);
	const double CurrentTotalThreadTime = HFiletimeMisc::ToSeconds(CurrentTime) - HFiletimeMisc::ToSeconds(ThreadCreationTime);

	const double IntervalThreadTime = CurrentTotalThreadTime - ThreadTimeInfo.LastTotalThreadTime;
	const double IntervalThreadUserAndKernelTime = CurrentTotalThreadUserAndKernelTime - ThreadTimeInfo.LastTotalThreadUserAndKernelTime;

	// IntervalUserAndKernelTime == 0.0f means that the OS hasn't updated the data yet, 
	// so don't update to avoid oscillating between 0 and calculated value.
	if (IntervalThreadUserAndKernelTime > 0.0)
	{
		HWindowsTimeInternal::CurrentThreadCPUStats.ThreadCPUTimePctRelative =
			(float)((IntervalThreadUserAndKernelTime / IntervalThreadTime) * 100.0);

		ThreadTimeInfo.LastTotalThreadTime = CurrentTotalThreadTime;
		ThreadTimeInfo.LastTotalThreadUserAndKernelTime = CurrentTotalThreadUserAndKernelTime;
		HWindowsTimeInternal::CurrentThreadCPUStats.LastIntervalThreadTime = IntervalThreadUserAndKernelTime;
	}

	return true;
}

//void HWindowsPlatformTime::AutoUpdateGameThreadCPUTime(double UpdateInterval)
//{
//	static bool bEnabledGameThreadTiming = false;
//
//	if (!bEnabledGameThreadTiming)
//	{
//		HTSTicker::GetCoreTicker().AddTicker(HTickerDelegate::CreateStatic(&HPlatformTime::UpdateThreadCPUTime), (float)UpdateInterval);
//
//		bEnabledGameThreadTiming = true;
//	}
//}

//HCPUTime HWindowsPlatformTime::GetCPUTime()
//{
//	return HCPUTime(CPUTimePctRelative / (float)HPlatformMisc::NumberOfCoresIncludingHyperthreads(), CPUTimePctRelative);
//}

// HCPUTime HWindowsPlatformTime::GetThreadCPUTime()
// {
// 	return HCPUTime(HWindowsTimeInternal::CurrentThreadCPUStats.ThreadCPUTimePctRelative /
// 		(float)HPlatformMisc::NumberOfCoresIncludingHyperthreads(),
// 		HWindowsTimeInternal::CurrentThreadCPUStats.ThreadCPUTimePctRelative);
// }

double HWindowsPlatformTime::GetLastIntervalThreadCPUTimeInSeconds()
{
	return HWindowsTimeInternal::CurrentThreadCPUStats.LastIntervalThreadTime;
}