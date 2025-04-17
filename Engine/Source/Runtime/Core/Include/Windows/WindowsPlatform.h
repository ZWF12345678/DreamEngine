#pragma once

#include "UniversalPlatform/UniversalPlatform.h"

// #include <sal.h>

//define the platform compiler
/*Complier Clang Define*/
#ifndef PLATFORM_COMPILER_CLANG
#if defined(__clang__)
#define PLATFORM_COMPILER_CLANG			1
#else
#define PLATFORM_COMPILER_CLANG			0
#endif // defined(__clang__)
#endif

/*Complier MSVC Define*/
#ifndef PLATFORM_COMPILER_MSVC
#if defined(_MSC_VER)
#define PLATFORM_COMPILER_MSVC			1
#else
#define PLATFORM_COMPILER_MSVC			0
#endif // defined(_MSC_VER)
#endif

/*Complier GCC/MINGW Define*/
#ifndef PLATFORM_COMPILER_MINGW
#if defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
#define PLATFORM_COMPILER_MINGW			1
#else
#define PLATFORM_COMPILER_MINGW			0
#endif // defined(__MINGW32__)
#endif


struct HWindowsPlatformDataType : public HUniversalPlatformDataType
{
#if defined(_WIN64)
#if !PLATFORM_COMPILER_MINGW
    typedef unsigned __int64		SIZE_T;
    typedef __int64					SSIZE_T;
    typedef unsigned __int64		UPTRINT;
#endif
#else
	typedef unsigned long			SIZE_T;
	typedef long					SSIZE_T;
	typedef unsigned long			UPTRINT;
#endif
};

typedef HWindowsPlatformDataType HPlatformDataType;

#if USE_UTF8_TCHARS
	typedef UTF8CHAR TCHAR;
#endif

#ifdef _WIN64
#define PLATFORM_CACHE_LINE_SIZE		64
#define PLATFORM_64BITS					1
#define PLATFORM_32BITS					0
#else
#define PLATFORM_CACHE_LINE_SIZE		32
#define PLATFORM_64BITS					0
#define PLATFORM_32BITS					1
#endif 

#if defined( _MANAGED ) || defined( _M_CEE )
	#define PLATFORM_COMPILER_COMMON_LANGUAGE_RUNTIME_COMPILATION	1
#endif

#ifndef VARARGS
#define VARARGS     __cdecl	
#endif // !VARARGS
#ifndef CDECL
#define CDECL		__cdecl
#endif // !CDECL
#ifndef STDCALL
#define STDCALL		__stdcall
#endif // !STDCALL
#ifndef FASTCALL
#define FASTCALL	__fastcall
#endif // !FASTCALL
#ifndef INLINE
#define INLINE		__inline
#endif // !INLINE
#ifndef FORCEINLINE
#ifdef __MINGW32__
#define FORCEINLINE inline
#else
#define FORCEINLINE __forceinline
#endif // !__MINGW32__
#endif // !FORCEINLINE
#ifndef FORCENOINLINE
#define FORCENOINLINE __declspec(noinline)
#endif // !FORCENOINLINE

//based defines
#define PLATFORM_LITTLE_ENDIAN								1
#define PLATFORM_SUPPORTS_UNALIGNED_LOADS					1
#define PLATFORM_SUPPORTS_FLIP_TRACKING						1

#define PLATFORM_DESKTOP									1
#define PLATFORM_LITTLE_ENDIAN								1
#define PLATFORM_SUPPORTS_UNALIGNED_LOADS					1
#define PLATFORM_SUPPORTS_FLIP_TRACKING						1
#define PLATFORM_CAN_SUPPORT_EDITORONLY_DATA				1

#define PLATFORM_SUPPORTS_PRAGMA_PACK						1

//CPU type defines
#if (defined(__arm__) || defined(_M_ARM) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC))
	#define PLATFORM_CPU_ARM_FAMILY							1
	#define PLATFORM_ENABLE_VECTORINTRINSICS_NEON			1
	#define PLATFORM_ENABLE_VECTORINTRINSICS				1
#elif (defined(_M_IX86) || defined(_M_X64))
	#define PLATFORM_CPU_ARM_FAMILY							0
	#define PLATFORM_CPU_X86_FAMILY							1
	#define PLATFORM_ENABLE_VECTORINTRINSICS				1
#endif

//Malloc define
#if PLATFORM_CPU_ARM_FAMILY
#define PLATFORM_SUPPORTS_TBB								0
#else
#define PLATFORM_SUPPORTS_TBB								1
#endif
#define PLATFORM_SUPPORTS_MIMALLOC							PLATFORM_64BITS

//Other defines
#define PLATFORM_HAS_BSD_TIME								0
#define PLATFORM_SUPPORTS_NAMED_PIPES						1
#define PLATFORM_COMPILER_HAS_TCHAR_WMAIN					1
#define PLATFORM_SUPPORTS_VIRTUAL_TEXTURE_STREAMING			1
#define PLATFORM_SUPPORTS_VARIABLE_RATE_SHADING				1
#define PLATFORM_SUPPORTS_MESH_SHADERS						1
#define PLATFORM_SUPPORTS_BINDLESS_RENDERING				1
#define PLATFORM_USES__ALIGNED_MALLOC						1
																

#define DECLARE_UINT64(x)	x 


#define PLATFORM_CODE_SECTION(Name) __declspec(code_seg(Name)) 

//Align and pack defines
#if defined(__clang__)
#define GCC_PACK(n) __attribute__((packed,aligned(n)))
#define GCC_ALIGN(n) __attribute__((aligned(n)))
#if defined(_MSC_VER)
#define MS_ALIGN(n) __declspec(align(n)) 
#endif
#else
#define MS_ALIGN(n) __declspec(align(n))
#endif

#if !defined(__clang__)
#define MSVC_PRAGMA(Pragma) __pragma(Pragma)
#endif

#define DLLEXPORT	__declspec(dllexport)
#define DLLIMPORT	__declspec(dllimport)