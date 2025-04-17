#pragma once

#include "UniversalPlatform/UniversalPlatformTime.h"

struct HWindowsPlatformTime : HUniversalPlatformTime
{
    //static CORE_API double InitTiming();

    static double Seconds();

    static UInt32 Cycles();

    static UInt64 Cycles64();

    static CORE_API void SystemTime( Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec );
	static CORE_API void UtcTime( Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec );

	static CORE_API bool UpdateCPUTime( float DeltaTime );
	static CORE_API bool UpdateThreadCPUTime(float = 0.0);
	// static CORE_API void AutoUpdateGameThreadCPUTime(double UpdateInterval);
	// static CORE_API HCPUTime GetCPUTime();
	// static CORE_API HCPUTime GetThreadCPUTime();
	static CORE_API double GetLastIntervalThreadCPUTimeInSeconds();

protected:

	/** Percentage CPU utilization for the last interval relative to one core. */
	static CORE_API float CPUTimePctRelative;
};

typedef HWindowsPlatformTime HPlatformTime;