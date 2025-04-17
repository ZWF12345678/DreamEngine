#pragma once

#include "CoreType.h"
#include "Template/IsPtr.h"
#include "Template/IsIntegral.h"

template <typename T>
FORCEINLINE constexpr T Align(T Val, UInt64 Alignment)
{
	static_assert(TIsIntegral<T>::Value || TIsPointer<T>::Value, "Align expects an integer or pointer type");

	return (T)(((UInt64)Val + Alignment - 1) & ~(Alignment - 1));
}

template <typename T>
FORCEINLINE constexpr T AlignDown(T Val, UInt64 Alignment)
{
	static_assert(TIsIntegral<T>::Value || TIsPointer<T>::Value, "AlignDown expects an integer or pointer type");

	return (T)(((UInt64)Val) & ~(Alignment - 1));
}

template <typename T>
FORCEINLINE constexpr bool IsAligned(T Val, UInt64 Alignment)
{
	static_assert(TIsIntegral<T>::Value || TIsPointer<T>::Value, "IsAligned expects an integer or pointer type");

	return !((UInt64)Val & (Alignment - 1));
}

template <typename T>
FORCEINLINE constexpr T AlignArbitrary(T Val, UInt64 Alignment)
{
	static_assert(TIsIntegral<T>::Value || TIsPointer<T>::Value, "AlignArbitrary expects an integer or pointer type");

	return (T)((((UInt64)Val + Alignment - 1) / Alignment) * Alignment);
}