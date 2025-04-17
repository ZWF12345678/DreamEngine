#pragma once

#include "CoreType.h"

#include<string>

#if PLATFORM_HAS_BSD_TIME 
	#include <sys/time.h> // IWYU pragma: export
#endif

/** Contains CPU utilization data. */
struct HCPUTime
{
	HCPUTime(float InCPUTimePct, float InCPUTimePctRelative)
		: CPUTimePct(InCPUTimePct)
		, CPUTimePctRelative(InCPUTimePctRelative)
	{
        
	}

	HCPUTime& operator+=(const HCPUTime& Other)
	{
		CPUTimePct += Other.CPUTimePct;
		CPUTimePctRelative += Other.CPUTimePctRelative;
		return *this;
	}

	float CPUTimePct;

	float CPUTimePctRelative;
};

/**
* Universal implementation for most platforms
**/

struct HUniversalPlatformTime
{
    #if PLATFORM_HAS_BSD_TIME 
	//The Section for Linux platform
	static CORE_API double InitTiming();

	static FORCEINLINE double Seconds()
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return ((double) tv.tv_sec) + (((double) tv.tv_usec) / 1000000.0);
	}

	static FORCEINLINE uint32 Cycles()
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return (UInt32) ((((UInt64)tv.tv_sec) * 1000000ULL) + (((UInt64)tv.tv_usec)));
	}
	static FORCEINLINE UInt64 Cycles64()
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return ((((UInt64)tv.tv_sec) * 1000000ULL) + (((UInt64)tv.tv_usec)));
	}

	/** Returns the system time. */
	static CORE_API void SystemTime( Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec );

	/** Returns the UTC time. */
	static CORE_API void UtcTime( Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec );
#endif
    /**
	 * Get the system date
     */

    static CORE_API ANSICHAR* StrDate( ANSICHAR* Dest, SIZE_T DestSize );

    /**
	 * Get the system time
     */

     static CORE_API ANSICHAR* StrTime( ANSICHAR* Dest, SIZE_T DestSize );

     /**
	 * Returns a timestamp string built from the current date and time.
	 * NOTE: Only one return value is valid at a time!
     */
    static CORE_API const ANSICHAR* StrTimestamp();

    /**
	 * Returns a pretty-string for a time given in seconds. ( "4:31 min", "2:16:30 hours")
	 */
    static CORE_API std::string PrettyTime( double Seconds );

    /** Updates CPU utilization, called through a delegate from the Core ticker. */
	static bool UpdateCPUTime( float DeltaTime )
	{
		return false;
	}

    /** Updates current thread CPU utilization, calling is user defined per-thread (unused float parameter, is for HTicker compatibility). */
	static bool UpdateThreadCPUTime(float = 0.0)
	{
		return false;
	}

    /** Registers automatic updates of Game Thread CPU utilization */
	static void AutoUpdateGameThreadCPUTime(double UpdateInterval)
	{
	}

    /**
	 * @return structure that contains CPU utilization data.
	 */
	static HCPUTime GetCPUTime()
	{
		return HCPUTime( 0.0f, 0.0f );
	}

    /**
	 * Gets current threads CPU Utilization
	 *
	 * @return	Current threads CPU Utilization
	 */
	static HCPUTime GetThreadCPUTime()
	{
		return HCPUTime(0.0f, 0.0f);
	}

    static double GetLastIntervalCPUTimeInSeconds()
	{
		return LastIntervalCPUTimeInSeconds;
	}

    static double GetLastIntervalThreadCPUTimeInSeconds()
	{
		return 0.0;
	}

    /**
	* @return seconds per cycle.
	*/
	static double GetSecondsPerCycle()
	{
		return SecondsPerCycle;
	}

    /** Converts cycles to milliseconds. */
	static float ToMilliseconds( const UInt32 Cycles )
	{
		return (float)double( SecondsPerCycle * 1000.0 * Cycles );
	}

    /** Converts cycles to seconds. */
	static float ToSeconds( const UInt32 Cycles )
	{
		return (float)double( SecondsPerCycle * Cycles );
	}

    /**
	 * @return seconds per cycle.
	 */
	static CORE_API double GetSecondsPerCycle64();

    /** Converts cycles to milliseconds. */
	static double ToMilliseconds64(const UInt64 Cycles)
	{
		return ToSeconds64(Cycles) * 1000.0;
	}

    /** Converts cycles to seconds. */
	static double ToSeconds64(const UInt64 Cycles)
	{
		return GetSecondsPerCycle64() * double(Cycles);
	}

protected:
	static CORE_API double SecondsPerCycle;
	static CORE_API double SecondsPerCycle64;
	static CORE_API double LastIntervalCPUTimeInSeconds;
};