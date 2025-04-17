#pragma once

#include "Math/MathUtility.h"

template <typename ElementType> struct TIntervalTraits;

namespace ETimespan
{
	inline constexpr Int64 MaxTicks = 9223372036854775807;

	inline constexpr Int64 MinTicks = -9223372036854775807 - 1;

	inline constexpr Int64 NanosecondsPerTick = 100;

	inline constexpr Int64 TicksPerDay = 864000000000;

	inline constexpr Int64 TicksPerHour = 36000000000;

	inline constexpr Int64 TicksPerMicrosecond = 10;

	inline constexpr Int64 TicksPerMillisecond = 10000;

	inline constexpr Int64 TicksPerMinute = 600000000;

	inline constexpr Int64 TicksPerSecond = 10000000;

	inline constexpr Int64 TicksPerWeek = 6048000000000;

	inline constexpr Int64 TicksPerYear = 365 * TicksPerDay;
}

struct HTimespan
{
public:

	HTimespan()
		: Ticks(0)
	{
	}

	HTimespan(Int64 InTicks)
		: Ticks(InTicks)
	{
        //TODO: Add log error message
		//KASSERT((InTicks >= ETimespan::MinTicks) && (InTicks <= ETimespan::MaxTicks));
	}

	HTimespan(Int32 Hours, Int32 Minutes, Int32 Seconds)
	{
		Assign(0, Hours, Minutes, Seconds, 0);
	}


	HTimespan(Int32 Days, Int32 Hours, Int32 Minutes, Int32 Seconds)
	{
		Assign(Days, Hours, Minutes, Seconds, 0);
	}


	HTimespan(Int32 Days, Int32 Hours, Int32 Minutes, Int32 Seconds, Int32 FractionNano)
	{
		Assign(Days, Hours, Minutes, Seconds, FractionNano);
	}

public:

	HTimespan operator+(const HTimespan& Other) const
	{
		return HTimespan(Ticks + Other.Ticks);
	}

	HTimespan& operator+=(const HTimespan& Other)
	{
		Ticks += Other.Ticks;
		return *this;
	}


	HTimespan operator-() const
	{
		return HTimespan(-Ticks);
	}

	
	HTimespan operator-(const HTimespan& Other) const
	{
		return HTimespan(Ticks - Other.Ticks);
	}

	
	HTimespan& operator-=(const HTimespan& Other)
	{
		Ticks -= Other.Ticks;
		return *this;
	}

	HTimespan operator*(double Scalar) const
	{
		return HTimespan((Int64)((double)Ticks * Scalar));
	}

	HTimespan& operator*=(double Scalar)
	{
		Ticks = (Int64)((double)Ticks * Scalar);
		return *this;
	}

	HTimespan operator/(double Scalar) const
	{
		return HTimespan((Int64)((double)Ticks / Scalar));
	}

	HTimespan& operator/=(double Scalar)
	{
		Ticks = (Int64)((double)Ticks / Scalar);
		return *this;
	}

	HTimespan operator%(const HTimespan& Other) const
	{
		return HTimespan(Ticks % Other.Ticks);
	}

	HTimespan& operator%=(const HTimespan& Other)
	{
		Ticks = Ticks % Other.Ticks;
		return *this;
	}

	bool operator==(const HTimespan& Other) const
	{
		return (Ticks == Other.Ticks);
	}


	bool operator!=(const HTimespan& Other) const
	{
		return (Ticks != Other.Ticks);
	}

	bool operator>(const HTimespan& Other) const
	{
		return (Ticks > Other.Ticks);
	}

	bool operator>=(const HTimespan& Other) const
	{
		return (Ticks >= Other.Ticks);
	}

	bool operator<(const HTimespan& Other) const
	{
		return (Ticks < Other.Ticks);
	}

	bool operator<=(const HTimespan& Other) const
	{
		return (Ticks <= Other.Ticks);
	}

public:

	/*CORE_API bool ExportTextItem(FString& ValueStr, HTimespan const& DefaultValue, UObject* Parent, Int32 PortFlags, UObject* ExportRootScope) const;*/

	Int32 GetDays() const
	{
		return (Int32)(Ticks / ETimespan::TicksPerDay);
	}

	HTimespan GetDuration()
	{
		return HTimespan(Ticks >= 0 ? Ticks : -Ticks);
	}

	Int32 GetFractionMicro() const
	{
		return (Int32)((Ticks % ETimespan::TicksPerSecond) / ETimespan::TicksPerMicrosecond);
	}

	Int32 GetFractionMilli() const
	{
		return (Int32)((Ticks % ETimespan::TicksPerSecond) / ETimespan::TicksPerMillisecond);
	}

	Int32 GetFractionNano() const
	{
		return (Int32)((Ticks % ETimespan::TicksPerSecond) * ETimespan::NanosecondsPerTick);
	}

	Int32 GetFractionTicks() const
	{
		return (Int32)(Ticks % ETimespan::TicksPerSecond);
	}

	Int32 GetHours() const
	{
		return (Int32)((Ticks / ETimespan::TicksPerHour) % 24);
	}

	Int32 GetMinutes() const
	{
		return (Int32)((Ticks / ETimespan::TicksPerMinute) % 60);
	}

	Int32 GetSeconds() const
	{
		return (Int32)((Ticks / ETimespan::TicksPerSecond) % 60);
	}

	Int64 GetTicks() const
	{
		return Ticks;
	}

	double GetTotalDays() const
	{
		return ((double)Ticks / ETimespan::TicksPerDay);
	}

	double GetTotalHours() const
	{
		return ((double)Ticks / ETimespan::TicksPerHour);
	}

	double GetTotalMicroseconds() const
	{
		return ((double)Ticks / ETimespan::TicksPerMicrosecond);
	}

	double GetTotalMilliseconds() const
	{
		return ((double)Ticks / ETimespan::TicksPerMillisecond);
	}

	double GetTotalMinutes() const
	{
		return ((double)Ticks / ETimespan::TicksPerMinute);
	}


	double GetTotalSeconds() const
	{
		return ((double)Ticks / ETimespan::TicksPerSecond);
	}


	/*CORE_API bool ImportTextItem(const TCHAR*& Buffer, Int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText);*/

	bool IsZero() const
	{
		return (Ticks == 0LL);
	}

	/*CORE_API bool Serialize(FArchive& Ar);*/

	
	/*CORE_API bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);*/

	/*CORE_API FString ToString() const;

	CORE_API FString ToString(const TCHAR* Format) const;*/

public:

	static HTimespan FromDays(double Days)
	{
		return HTimespan((Int64)HMath::FloorToDouble(Days * ETimespan::TicksPerDay + 0.5));
	}

	static HTimespan FromHours(double Hours)
	{
		return HTimespan((Int64)HMath::FloorToDouble(Hours * ETimespan::TicksPerHour + 0.5));
	}

	static HTimespan FromMicroseconds(double Microseconds)
	{
		return HTimespan((Int64)HMath::FloorToDouble(Microseconds * ETimespan::TicksPerMicrosecond + 0.5));
	}

	static HTimespan FromMilliseconds(double Milliseconds)
	{
		return HTimespan((Int64)HMath::FloorToDouble(Milliseconds * ETimespan::TicksPerMillisecond + 0.5));
	}


	static HTimespan FromMinutes(double Minutes)
	{
		return HTimespan((Int64)HMath::FloorToDouble(Minutes * ETimespan::TicksPerMinute + 0.5));
	}

	static HTimespan FromSeconds(double Seconds)
	{
		return HTimespan((Int64)HMath::FloorToDouble(Seconds * ETimespan::TicksPerSecond + 0.5));
	}

	static HTimespan MaxValue()
	{
		return HTimespan(ETimespan::MaxTicks);
	}


	static HTimespan MinValue()
	{
		return HTimespan(ETimespan::MinTicks);
	}

	//static CORE_API bool Parse(const FString& TimespanString, HTimespan& OutTimespan);

	static double Ratio(HTimespan Dividend, HTimespan Divisor)
	{
		if (Divisor == HTimespan::Zero())
		{
			return 0.0;
		}

		return (double)Dividend.GetTicks() / (double)Divisor.GetTicks();
	}

	static HTimespan Zero()
	{
		return HTimespan(0);
	}

public:

	friend class UObject;

	/*friend CORE_API FArchive& operator<<(FArchive& Ar, HTimespan& Timespan);*/

	/*friend CORE_API UInt32 GetTypeHash(const HTimespan& Timespan);*/

protected:
    //TODO: Implement this function
	void CORE_API Assign(Int32 Days, Int32 Hours, Int32 Minutes, Int32 Seconds, Int32 FractionNano);

private:
	friend struct Z_Construct_UScriptStruct_HTimespan_Statics;

private:

	/** The time span value in 100 nanoseconds resolution. */
	Int64 Ticks;
};

template <>
struct TIntervalTraits<HTimespan>
{
	static HTimespan Max()
	{
		return HTimespan::MaxValue();
	}

	static HTimespan Lowest()
	{
		return HTimespan::MinValue();
	}
};

inline HTimespan operator*(float Scalar, const HTimespan& Timespan)
{
	return Timespan.operator*(Scalar);
}

