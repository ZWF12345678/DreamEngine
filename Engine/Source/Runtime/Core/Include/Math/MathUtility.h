#pragma once

#include "CoreType.h"
#include "Template/ResolveTypeAmbiguity.h"

#include <cmath>

#define DE_DELTA		(0.00001f)
#define DE_DOUBLE_DELTA	(0.00001 )

#define DE_PI 					(3.1415926535897932f)
#define DE_SMALL_NUMBER			(1.e-8f)
#define DE_KINDA_SMALL_NUMBER	(1.e-4f)
#define DE_BIG_NUMBER			(3.4e+38f)
#define DE_EULERS_NUMBER		(2.71828182845904523536f)
#define DE_GOLDEN_RATIO			(1.6180339887498948482045868343656381f)
#define DE_FLOAT_NON_FRACTIONAL (8388608.f)


#define DE_DOUBLE_PI					(3.141592653589793238462643383279502884197169399)
#define DE_DOUBLE_SMALL_NUMBER			(1.e-8)
#define DE_DOUBLE_KINDA_SMALL_NUMBER	(1.e-4)
#define DE_DOUBLE_BIG_NUMBER			(3.4e+38)
#define DE_DOUBLE_EULERS_NUMBER			(2.7182818284590452353602874713526624977572)
#define DE_DOUBLE_GOLDEN_RATIO			(1.6180339887498948482045868343656381)
#define DE_DOUBLE_NON_FRACTIONAL		(4503599627370496.0)


template <typename T>
struct TCustomLerp
{
	constexpr static bool Value = false;
};


class HMath
{
private:
	static CORE_API void FmodReportError(float X, float Y);
	static CORE_API void FmodReportError(double X, double Y);

public:
	static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
	static FORCEINLINE double Sqrt(double Value) { return sqrt(Value); }
	static CORE_API FORCENOINLINE float Fmod(float X, float Y);
	static CORE_API FORCENOINLINE double Fmod(double X, double Y);

	static FORCEINLINE float Sin(float Value) { return sinf(Value); }
	static FORCEINLINE double Sin(double Value) { return sin(Value); }

	static FORCEINLINE float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE double Asin(double Value) { return asin((Value < -1.0) ? -1.0 : ((Value < 1.0) ? Value : 1.0)); }

	static FORCEINLINE float Sinh(float Value) { return sinhf(Value); }
	static FORCEINLINE double Sinh(double Value) { return sinh(Value); }

	static FORCEINLINE float Cos(float Value) { return cosf(Value); }
	static FORCEINLINE double Cos(double Value) { return cos(Value); }

	static FORCEINLINE float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE double Acos(double Value) { return acos((Value < -1.0) ? -1.0 : ((Value < 1.0) ? Value : 1.0)); }

	static FORCEINLINE float Cosh(float Value) { return coshf(Value); }
	static FORCEINLINE double Cosh(double Value) { return cosh(Value); }

	static FORCEINLINE float Tan(float Value) { return tanf(Value); }
	static FORCEINLINE double Tan(double Value) { return tan(Value); }

	static FORCEINLINE float Atan(float Value) { return atanf(Value); }
	static FORCEINLINE double Atan(double Value) { return atan(Value); }

	static FORCEINLINE float Tanh(float Value) { return tanhf(Value); }
	static FORCEINLINE double Tanh(double Value) { return tanh(Value); }


public:
	static FORCEINLINE Int32 Rand() { return rand(); }
	static FORCEINLINE Int32 Rand32() { return ((rand() & 0x7fff) << 16) | ((rand() & 0x7fff) << 1) | (rand() & 0x1); }

	static FORCEINLINE void RandInit(Int32 Seed) { srand(Seed); }

	static FORCEINLINE float FRand()
	{
		constexpr Int32 RandMax = 0x00ffffff < RAND_MAX ? 0x00ffffff : RAND_MAX;
		return (Rand() & RandMax) / (float)RandMax;
	}

	static constexpr FORCEINLINE Int32 TruncToInt32(float F)
	{
		return (Int32)F;
	}
	static constexpr FORCEINLINE Int32 TruncToInt32(double F)
	{
		return (Int32)F;
	}
	static constexpr FORCEINLINE Int64 TruncToInt64(double F)
	{
		return (Int64)F;
	}

	static constexpr FORCEINLINE Int32 TruncToInt(float F) { return TruncToInt32(F); }
	static constexpr FORCEINLINE Int64 TruncToInt(double F) { return TruncToInt64(F); }
public:
	static FORCEINLINE Int32 CeilToInt32(float F)
	{
		Int32 I = TruncToInt32(F);
		I += ((float)I < F);
		return I;
	}

	static FORCEINLINE Int32 CeilToInt32(double F)
	{
		Int32 I = TruncToInt32(F);
		I += ((double)I < F);
		return I;
	}

	static FORCEINLINE Int64 CeilToInt64(double F)
	{
		Int64 I = TruncToInt64(F);
		I += ((double)I < F);
		return I;
	}

	static FORCEINLINE double FloorToDouble(double F)
	{
		return floor(F);
	}

	static FORCEINLINE Int32 CeilToInt(float F) { return CeilToInt32(F); }
	static FORCEINLINE Int64 CeilToInt(double F) { return CeilToInt64(F); }

	template <class T>
	[[nodiscard]] static constexpr inline T DivideAndRoundUp(T Dividend, T Divisor)
	{
		return (Dividend + Divisor - 1) / Divisor;
	}

	template< class T >
	static constexpr FORCEINLINE T Min(const T A, const T B)
	{
		return (A < B) ? A : B;
	}

	template< class T >
	static constexpr FORCEINLINE T Max(const T A, const T B)
	{
		return (B < A) ? A : B;
	}

	template< class T >
	[[nodiscard]] static constexpr FORCEINLINE T Clamp(const T X, const T MinValue, const T MaxValue)
	{
		return Max(Min(X, MaxValue), MinValue);
	}

	template< class T >
	static constexpr FORCEINLINE T Abs(const T A)
	{
		return (A < (T)0) ? -A : A;
	}

	template< class T >
	[[nodiscard]] static constexpr FORCEINLINE T Square(const T A)
	{
		return A * A;
	}

	template< class T >
	[[nodiscard]] static constexpr FORCEINLINE T Min3(const T A, const T B, const T C)
	{
		return Min(Min(A, B), C);
	}

	template< class T >
	[[nodiscard]] static constexpr FORCEINLINE T Max3(const T A, const T B, const T C)
	{
		return Max<T>(Max<T>(A, B), C);
	}

	template <
		typename T,
		typename U
		DE_REQUIRES(!TCustomLerp<T>::Value && (std::is_floating_point_v<U> || std::is_same_v<T, U>) && !std::is_same_v<T, bool>)
	>
	[[nodiscard]] static constexpr FORCEINLINE T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}

	/** Custom lerps defined for those classes not suited to the default implemenation. */
	template <
		typename T,
		typename U
		DE_REQUIRES(TCustomLerp<T>::Value)
	>
	[[nodiscard]] static FORCEINLINE T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return TCustomLerp<T>::Lerp(A, B, Alpha);
	}

	// Allow passing of differing A/B types.
	template <
		typename T1,
		typename T2,
		typename T3
		DE_REQUIRES(!std::is_same_v<T1, T2> && !TCustomLerp<T1>::Value && !TCustomLerp<T2>::Value)
	>
	[[nodiscard]] static auto Lerp(const T1& A, const T2& B, const T3& Alpha) -> decltype(A* B)
	{
		using ABType = decltype(A* B);
		return Lerp(ABType(A), ABType(B), Alpha);
	}
};

template<>
FORCEINLINE float HMath::Abs(const float A)
{
	return fabsf(A);
}
template<>
FORCEINLINE double HMath::Abs(const double A)
{
	return fabs(A);
}