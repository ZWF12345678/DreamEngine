#include "Common/PlatformTime.h"
#include "Math/MathUtility.h"
#include "UniversalPlatform/UniversalPlatformTime.h"

#include <string.h>

double HUniversalPlatformTime::SecondsPerCycle = 0.0;
double HUniversalPlatformTime::SecondsPerCycle64 = 0.0;
double HUniversalPlatformTime::LastIntervalCPUTimeInSeconds = 0.0;

double HUniversalPlatformTime::GetSecondsPerCycle64()
{
    //TODO
	//KASSERT(SecondsPerCycle64 != 0.0);
	return SecondsPerCycle64;
}

#if  PLATFORM_HAS_BSD_TIME 
// #include <sched.h>

double HUniversalPlatformTime::InitTiming()
{
    SecondsPerCycle = 1.0f / 1000000.0f;
	SecondsPerCycle64 = 1.0 / 1000000.0;

    return HPlatformTime::Seconds();
}

void HUniversalPlatformTime::SystemTime( Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec )
{
    // query for calendar time
	struct timeval Time;
	gettimeofday(&Time, NULL);

	// convert it to local time
	struct tm LocalTime;
	localtime_r(&Time.tv_sec, &LocalTime);

	// pull out data/time
	Year		= LocalTime.tm_year + 1900;
	Month		= LocalTime.tm_mon + 1;
	DayOfWeek	= LocalTime.tm_wday;
	Day			= LocalTime.tm_mday;
	Hour		= LocalTime.tm_hour;
	Min			= LocalTime.tm_min;
	Sec			= LocalTime.tm_sec;
	MSec		= Time.tv_usec / 1000;
}

void HUniversalPlatformTime::UtcTime( Int32& Year, Int32& Month, Int32& DayOfWeek, Int32& Day, Int32& Hour, Int32& Min, Int32& Sec, Int32& MSec )
{
    // query for calendar time
	struct timeval Time;
	gettimeofday(&Time, NULL);

	// convert it to UTC
	struct tm LocalTime;
	gmtime_r(&Time.tv_sec, &LocalTime);

	// pull out data/time
	Year		= LocalTime.tm_year + 1900;
	Month		= LocalTime.tm_mon + 1;
	DayOfWeek	= LocalTime.tm_wday;
	Day			= LocalTime.tm_mday;
	Hour		= LocalTime.tm_hour;
	Min			= LocalTime.tm_min;
	Sec			= LocalTime.tm_sec;
	MSec		= Time.tv_usec / 1000;
}

#endif

ANSICHAR* HUniversalPlatformTime::StrDate( ANSICHAR* Dest, SIZE_T DestSize )
{
    Int32 Year;
	Int32 Month;
	Int32 DayOfWeek;
	Int32 Day;
	Int32 Hour;
	Int32 Min;
	Int32 Sec;
	Int32 MSec;

    HPlatformTime::SystemTime(Year,Month,DayOfWeek,Day,Hour,Min,Sec,MSec);

    snprintf(Dest,DestSize,"%02d/%02d/%02d", Month, Day, Year % 100);

    return Dest;
}



ANSICHAR* HUniversalPlatformTime::StrTime( ANSICHAR* Dest, SIZE_T DestSize )
{
    Int32 Year;
	Int32 Month;
	Int32 DayOfWeek;
	Int32 Day;
	Int32 Hour;
	Int32 Min;
	Int32 Sec;
	Int32 MSec;

    HPlatformTime::SystemTime(Year,Month,DayOfWeek,Day,Hour,Min,Sec,MSec);

    snprintf(Dest,(Int32)DestSize,"%02d:%02d:%02d",  Hour, Min, Sec);

    return Dest;
}

const ANSICHAR* HUniversalPlatformTime::StrTimestamp()
{
    static thread_local ANSICHAR Result[1024];
    *Result = '\0';

    StrDate(Result,DE_ARRAY_COUNT(Result));
    strcat(Result," ");
    StrTime(Result + strlen(Result), DE_ARRAY_COUNT(Result) - strlen(Result));

    return Result;
}

std::string HUniversalPlatformTime::PrettyTime( double Seconds )
{

    ANSICHAR Buffer[128];
    memset(Buffer,0,128);

    if (Seconds < 0.001)
	{
        int Written = std::snprintf(Buffer, 128, "%d us", HMath::TruncToInt((float)(Seconds*1000000)));
        if (Written < 0 || static_cast<size_t>(Written) >= 128)
        {
            return std::string("Error: Output truncated or invalid format.");
        }
		return std::string(Buffer);
	}
	else if ( Seconds < 1.0 )
	{
		Int32 Ms = HMath::TruncToInt((float)(Seconds * 1000));
		Int32 Us = HMath::TruncToInt((float)(Seconds * 1000000)) - Ms*1000;

        int Written = std::snprintf(Buffer, 128, "%d.%03d ms",  Ms, Us);
        if (Written < 0 || static_cast<size_t>(Written) >= 128)
        {
            return std::string("Error: Output truncated or invalid format.");
        }

		return std::string(Buffer);
	}
	else if ( Seconds < 10.0 )
	{
		Int32 Sec = HMath::TruncToInt((float)Seconds);
		Int32 Ms = HMath::TruncToInt((float)(Seconds*1000)) - Sec*1000;
        
        int Written = std::snprintf(Buffer, 128, "%d.%02d sec",  Sec, Ms/10);
        if (Written < 0 || static_cast<size_t>(Written) >= 128)
        {
            return std::string("Error: Output truncated or invalid format.");
        }

		return std::string(Buffer);
	}
	else if ( Seconds < 60.0 )
	{
		Int32 Sec = HMath::TruncToInt((float)Seconds);
		Int32 Ms = HMath::TruncToInt((float)(Seconds*1000)) - Sec*1000;
        
        int Written = std::snprintf(Buffer, 128, "%d.%d sec",  Sec, Ms/100);
        if (Written < 0 || static_cast<size_t>(Written) >= 128)
        {
            return std::string("Error: Output truncated or invalid format.");
        }
		return std::string(Buffer);
	}
	else if ( Seconds < 60.0*60.0 )
	{
		Int32 Min = HMath::TruncToInt((float)(Seconds/60.0));
		Int32 Sec = HMath::TruncToInt((float)Seconds) - Min*60;

        int Written = std::snprintf(Buffer, 128, "%d:%02d min",  Min, Sec);
        if (Written < 0 || static_cast<size_t>(Written) >= 128)
        {
            return std::string("Error: Output truncated or invalid format.");
        }
		return std::string(Buffer);
	}
	else
	{
		Int32 Hr = HMath::TruncToInt((float)(Seconds/3600.0));
		Int32 Min = HMath::TruncToInt((float)((Seconds - Hr*3600)/60.0));
		Int32 Sec = HMath::TruncToInt((float)(Seconds - Hr*3600 - Min*60));

        int Written = std::snprintf(Buffer, 128, "%d:%02d:%02d hours",   Hr, Min, Sec );
        if (Written < 0 || static_cast<size_t>(Written) >= 128)
        {
            return std::string("Error: Output truncated or invalid format.");
        }
		return std::string(Buffer);
	}
}
