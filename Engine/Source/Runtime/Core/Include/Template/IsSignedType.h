#pragma once

#include "CoreType.h"

template <typename T>
struct TIsSigned
{
	enum { Value = false };
};

template <> struct TIsSigned<Int8>	{ enum { Value = true }; };
template <> struct TIsSigned<Int16> { enum { Value = true }; };
template <> struct TIsSigned<Int32> { enum { Value = true }; };
template <> struct TIsSigned<Int64> { enum { Value = true }; };

template <typename T> struct TIsSigned<const          T> { enum { Value = TIsSigned<T>::Value }; };
template <typename T> struct TIsSigned<      volatile T> { enum { Value = TIsSigned<T>::Value }; };
template <typename T> struct TIsSigned<const volatile T> { enum { Value = TIsSigned<T>::Value }; };

