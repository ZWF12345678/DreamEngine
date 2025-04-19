#pragma once

/*
 *This part is mainly about the early definition of different platforms, as well as testing the size of platform data types.
*/

#include "Common/PreProcessHelper.h"

//OS platform defines
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define PLATFORM_WINDOWS 	1
#elif defined(__unix__) || defined(__unix)
#define PLATFORM_UNIX 		1
#elif defined(__linux__)
#define PLATFORM_LINUX 		1
#elif defined(__APPLE__)
#define PLATFORM_MAC 		1
#elif defined(ANDROID) || defined(_ANDROID_)
#define PLATFORM_ANDROID 	1
#else
#error "Unsupported platform"
#endif

//Different platform's include header file
#if PLATFORM_WINDOWS
#define PLATFORM_HEADER_NAME Windows
#elif PLATFORM_UNIX
#define PLATFORM_HEADER_NAME Unix
#elif PLATFORM_LINUX
#define PLATFORM_HEADER_NAME Linux
#elif PLATFORM_MAC
#define PLATFORM_HEADER_NAME Mac
#elif PLATFORM_ANDROID
#define PLATFORM_HEADER_NAME Android
#else
#error "Unsupported platform"
#endif


#include COMPILED_PLATFORM_HEADER(Platform.h)

#define PLATFORM_USE_SSE2_FOR_THREAD_YIELD	PLATFORM_CPU_X86_FAMILY


//Some of defines about char types:TCHAR、WCHAR、UTF8CHAR、UCS2CHAR

#ifndef PLATFORM_TCHAR_IS_4_BYTES
#define PLATFORM_TCHAR_IS_4_BYTES			0
#endif

#ifndef PLATFORM_TCHAR_IS_CHAR16
#define PLATFORM_TCHAR_IS_CHAR16			0
#endif

#ifndef PLATFORM_TCHAR_IS_UTF8CHAR
#define PLATFORM_TCHAR_IS_UTF8CHAR			USE_UTF8_TCHARS
#endif

#ifndef PLATFORM_WCHAR_IS_4_BYTES
#define PLATFORM_WCHAR_IS_4_BYTES			0
#endif

#define PLATFORM_WIDECHAR_IS_CHAR16 		PLATFORM_TCHAR_IS_CHAR16

#ifndef PLATFORM_UCS2CHAR_IS_UTF16CHAR
#define PLATFORM_UCS2CHAR_IS_UTF16CHAR		1
#endif

// Base Data Type Definition
typedef HPlatformDataType::UInt8 			UInt8;

typedef HPlatformDataType::UInt8			Byte;

typedef HPlatformDataType::UInt16			UInt16;

typedef HPlatformDataType::UInt32			UInt32;

typedef	HPlatformDataType::UInt64			UInt64;

typedef	HPlatformDataType::Int8				Int8;

typedef HPlatformDataType::Int16			Int16;

typedef HPlatformDataType::Int32            Int32;

typedef HPlatformDataType::Int64            Int64;

typedef HPlatformDataType::ANSICHAR         ANSICHAR;		//It is a fixed 8 bytes in memory, independent of different platforms.

typedef HPlatformDataType::WIDECHAR         WIDECHAR;		//The number of bits in memory depends on the different platforms.

typedef HPlatformDataType::UTF8CHAR         UTF8CHAR;

typedef HPlatformDataType::CHAR16           UCS2CHAR;

typedef HPlatformDataType::CHAR16           UTF16CHAR;

typedef HPlatformDataType::CHAR32			UTF32CHAR;

// typedef HPlatformDataType::TCHAR            TCHAR;

typedef HPlatformDataType::TYPE_OF_NULL     TYPE_OF_NULL;

typedef HPlatformDataType::TYPE_OF_NULLPTR  TYPE_OF_NULLPTR;

typedef HPlatformDataType::SIZE_T           SIZE_T;

typedef HPlatformDataType::SSIZE_T          SSIZE_T;

typedef HPlatformDataType::UPTRINT          UPTRINT;

typedef HPlatformDataType::SPTRINT          SPTRINT;


namespace TypesTest
{
	template <typename T1, typename T2>
	inline constexpr bool TIsSameType = false;

	template <typename T>
	inline constexpr bool TIsSameType<T, T> = true;


	static_assert(char(-1) < char(0), "UChar test failed.");

#if PLATFORM_TCHAR_IS_4_BYTES
	static_assert(sizeof(TCHAR) == 4, "TCHAR size must be 4 bytes.");
#elif PLATFORM_TCHAR_IS_UTF8CHAR
	static_assert(sizeof(TCHAR) == 1, "TCHAR size must be 1 byte.");
#else
	// static_assert(sizeof(TCHAR) == 2, "TCHAR size must be 2 bytes.");
#endif

	static_assert(!PLATFORM_WCHAR_IS_4_BYTES || sizeof(wchar_t) == 4, "wchar_t size must be 4 bytes.");
	static_assert(PLATFORM_WCHAR_IS_4_BYTES || sizeof(wchar_t) == 2, "wchar_t size must be 2 bytes.");

	static_assert(PLATFORM_32BITS || PLATFORM_64BITS, "Tests Ptr size failed.");
	static_assert(PLATFORM_32BITS != PLATFORM_64BITS, "Tests Ptr exclusive failed.");
	static_assert(!PLATFORM_64BITS || sizeof(void*) == 8, "Ptr size is 64bit, but Ptr size are short.");
	static_assert(PLATFORM_64BITS || sizeof(void*) == 4, "Ptr size is 32bit, but Ptr size are long.");


	static_assert((!TIsSameType<ANSICHAR, WIDECHAR>), "ANSICHAR and WIDECHAR should be different types.");
	static_assert((!TIsSameType<ANSICHAR, UTF8CHAR>), "ANSICHAR and UTF8CHAR should be different types.");
#if !PLATFORM_UCS2CHAR_IS_UTF16CHAR
	static_assert((!TIsSameType<UCS2CHAR, UTF16CHAR>), "UCS2CHAR and UTF16CHAR should be different types.");
#else
	static_assert(TIsSameType<UCS2CHAR, UTF16CHAR>, "UCS2CHAR and UTF16CHAR are expected to be the same type.");
#endif
	static_assert((!TIsSameType<ANSICHAR, UCS2CHAR>), "ANSICHAR and UCS2CHAR should be different types.");
	static_assert((!TIsSameType<WIDECHAR, UCS2CHAR>), "WIDECHAR and UCS2CHAR should be different types.");
	// static_assert(TIsSameType<TCHAR, ANSICHAR> || TIsSameType<TCHAR, WIDECHAR> || TIsSameType<TCHAR, UTF8CHAR>, "TCHAR should either be ANSICHAR, WIDECHAR or UTF8CHAR.");

#if PLATFORM_WIDECHAR_IS_CHAR16
	static_assert(TIsSameType<WIDECHAR, char16_t>, "WIDECHAR should be char16_t");
#else
	static_assert(TIsSameType<WIDECHAR, wchar_t>, "WIDECHAR should be wchar_t");
#endif

	static_assert(sizeof(UInt8) == 1, "UInt8 size test failed.");
	static_assert(Int32(UInt8(-1)) == 0xFF, "UInt8 signed test failed.");

	static_assert(sizeof(UInt16) == 2, "UInt16 size test failed.");
	static_assert(Int32(UInt16(-1)) == 0xFFFF, "Uint16 signed test failed.");

	static_assert(sizeof(UInt32) == 4, "UInt32 size test failed.");
	static_assert(Int64(UInt32(-1)) == 0xFFFFFFFF, "UInt32 signed test failed.");

	static_assert(sizeof(UInt64) == 8, "UInt64 size test failed.");
	static_assert(UInt64(-1) > UInt64(0), "UInt64 signed test failed.");


	static_assert(sizeof(Int8) == 1, "Int8 size test failed.");
	static_assert(Int32(Int8(-1)) == -1, "Int8 sign test failed.");

	static_assert(sizeof(Int16) == 2, "Int16 size test failed.");
	static_assert(Int32(Int16(-1)) == -1, "Int16 signed test failed.");

	static_assert(sizeof(Int32) == 4, "Int32 size test failed.");
	static_assert(Int64(Int32(-1)) == Int64(-1), "Int32 signed test failed.");

	static_assert(sizeof(Int64) == 8, "Int64 size test failed.");
	static_assert(Int64(-1) < Int64(0), "Int64 signed test failed.");

	static_assert(sizeof(ANSICHAR) == 1, "ANSICHAR size test failed.");
	static_assert(Int32(ANSICHAR(-1)) == -1, "ANSICHAR type sign test failed.");

	static_assert(sizeof(WIDECHAR) == 2 || sizeof(WIDECHAR) == 4, "WIDECHAR size test failed.");

	static_assert(sizeof(UTF8CHAR) == 1, "UTF8CHAR size test failed.");
	static_assert(sizeof(UCS2CHAR) == 2, "UCS2CHAR size test failed.");
	static_assert(sizeof(UTF16CHAR) == 2, "UTF16CHAR size test failed.");
	static_assert(sizeof(UTF32CHAR) == 4, "UTF32CHAR  size test failed.");

	static_assert(sizeof(SPTRINT) == sizeof(void*), "SPTRINT size test failed.");
	static_assert(SPTRINT(-1) < SPTRINT(0), "SPTRINT signed test failed.");

	static_assert(sizeof(UPTRINT) == sizeof(void*), "UPTRINT size test failed.");
	static_assert(UPTRINT(-1) > UPTRINT(0), "UPTRINT signed test failed.");

	static_assert(sizeof(SIZE_T) == sizeof(void*), "SIZE_T size test failed.");
	static_assert(SIZE_T(-1) > SIZE_T(0), "SIZE_T signed test failed.");

}

namespace Private
{
	template <SIZE_T N>
	FORCEINLINE auto ToUTF8Literal(const char(&Array)[N]) -> const UTF8CHAR(&)[N]
	{
		return (const UTF8CHAR(&)[N])Array;
	}

#if defined(__cpp_char8_t)
	template <SIZE_T N>
	FORCEINLINE auto ToUTF8Literal(const char8_t(&Array)[N]) -> const UTF8CHAR(&)[N]
	{
		return (const UTF8CHAR(&)[N])Array;
	}
#endif

	FORCEINLINE constexpr UTF8CHAR ToUTF8Literal(unsigned long long Ch)
	{
		return (UTF8CHAR)Ch;
	}

	template <typename CharType>
	void CharTextStaticAssert()
	{
		static_assert(sizeof(CharType) == 0, "Unsupported char type passed to CHARTEXT");
	}
}

#define UTF8TEXT_PASTE(x)  u8 ## x
#define UTF16TEXT_PASTE(x) u ## x

#if PLATFORM_WIDECHAR_IS_CHAR16
#define WIDETEXT_PASTE(x)  UTF16TEXT_PASTE(x)
#else
#define WIDETEXT_PASTE(x)  L ## x
#endif

#define UTF8TEXT(x) (Private::ToUTF8Literal(UTF8TEXT_PASTE(x)))
#define UTF16TEXT(x) UTF16TEXT_PASTE(x)
#define WIDETEXT(str) WIDETEXT_PASTE(str)

#if !defined(DE_TEXT)
#if PLATFORM_TCHAR_IS_UTF8CHAR
#define TEXT_PASTE(x) UTF8TEXT(x)
#else
#define TEXT_PASTE(x) WIDETEXT(x)
#endif
#define DE_TEXT(x) TEXT_PASTE(x)
#endif

//NOTE : It has some problems with the TCHAR type, so it is not used for the time being.