#pragma once

/*
 *This section defines some common platform data types, including shape types, floating-point types, character types, and pointer sizes.
*/

//Help us to automatically determine the pointer size.
template<typename PtrSize32, typename PtrSize64, int PtrSize3>
struct TPointerSizeHelper
{

};

//It will be selected when the platform pointer is 8 bytes
template<typename PtrSize32, typename PtrSize64>
struct TPointerSizeHelper<PtrSize32, PtrSize64, 8>
{
	typedef PtrSize64 PointerSizeType;
};

//It will be selected when the platform pointer is 4 bytes
template<typename PtrSize32, typename PtrSize64>
struct TPointerSizeHelper<PtrSize32, PtrSize64, 4>
{
	typedef PtrSize32 PointerSizeType;
};

//Some data definitions about generic platforms
struct HUniversalPlatformDataType
{
	typedef unsigned char 				UInt8;

	typedef unsigned short int			UInt16;

	typedef unsigned int				UInt32;

	typedef unsigned long long			UInt64;

	typedef	signed char					Int8;

	typedef signed short int			Int16;

	typedef signed int	 				Int32;

	typedef signed long long			Int64;

	typedef char						ANSICHAR; 		//It is a fixed 8 bytes in memory, independent of different platforms.

	typedef wchar_t						WIDECHAR; 		//The number of bits in memory depends on the different platforms(e.g It occupies 2 bytes under the Windows platform and 4 bytes under the Linux platform.).

	typedef UInt8						UTF8CHAR;

	typedef UInt16						CHAR16;

	typedef UInt32						CHAR32;

	typedef WIDECHAR					TCHAR;

	typedef Int32						TYPE_OF_NULL;

	typedef decltype(nullptr)			TYPE_OF_NULLPTR;

	typedef TPointerSizeHelper<UInt32, UInt64, sizeof(void*)>::PointerSizeType UPTRINT;

	typedef TPointerSizeHelper<Int32, Int64, sizeof(void*)>::PointerSizeType SPTRINT;

	typedef UPTRINT SIZE_T;

	typedef SPTRINT SSIZE_T;

};