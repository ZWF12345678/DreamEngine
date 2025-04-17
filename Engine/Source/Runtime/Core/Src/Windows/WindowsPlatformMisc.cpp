
#if defined(__MINGW32__) || defined(__GNUC__)
#pragma GCC target("waitpkg")
#endif

#include "Windows/WindowsPlatformMisc.h"

#include <intrin.h>
#include <windows.h>

class HCPUIDQueriedData
{
public:
	HCPUIDQueriedData()
		: bHasCPUIDInstruction(CheckForCPUIDInstruction()), Vendor(), CPUInfo(0), CacheLineSize(PLATFORM_CACHE_LINE_SIZE)
	{
		bHasTimedPauseInstruction = false;
		if (bHasCPUIDInstruction)
		{
			GetCPUVendor(Vendor);
			GetCPUBrand(Brand);
			int Info[4];
			QueryCPUInfo(Info);
			CPUInfo = Info[0];
			CPUInfo2 = Info[2];
			CacheLineSize = QueryCacheLineSize();
			bHasTimedPauseInstruction = CheckForTimedPauseInstruction();
		}
	}

	static bool HasCPUIDInstruction()
	{
		return CPUIDStaticCache.bHasCPUIDInstruction;
	}

	static bool HasTimedPauseInstruction()
	{
		return CPUIDStaticCache.bHasTimedPauseInstruction;
	}


	static const ANSICHAR(&GetVendor())[12 + 1]
	{
		return CPUIDStaticCache.Vendor;
	}


	static const ANSICHAR(&GetBrand())[0x40]
	{
		return CPUIDStaticCache.Brand;
	}


	static UInt32 GetCPUInfo()
	{
		return CPUIDStaticCache.CPUInfo;
	}

	static UInt32 GetCPUInfo2()
	{
		return CPUIDStaticCache.CPUInfo2;
	}

	
	static Int32 GetCacheLineSize()
	{
		return CPUIDStaticCache.CacheLineSize;
	}

private:

#if !PLATFORM_CPU_ARM_FAMILY
	/**
	 * Checks if __cpuid instruction is present on current machine.
	 *
	 * @returns True if this CPU supports __cpuid instruction. False otherwise.
	 */
	static bool CheckForCPUIDInstruction()
	{
		// all x86 64-bit CPUs support CPUID, no check required
#if PLATFORM_HAS_CPUID && PLATFORM_64BITS
		return true;
#else
#if PLATFORM_SEH_EXCEPTIONS_DISABLED
		return false;
#else
		try
		{
			int Args[4];
			__cpuid(Args, 0);
		}
		catch (...)
		{
			return false;
		}
		return true;
#endif
#endif
	}

	static int FilterInvalidOpcode(DWORD ExceptionCode, struct _EXCEPTION_POINTERS* ExceptionInformation)
	{
		if (ExceptionCode == STATUS_ILLEGAL_INSTRUCTION || ExceptionCode == STATUS_PRIVILEGED_INSTRUCTION)
		{
			return EXCEPTION_EXECUTE_HANDLER;
		}
		else
		{
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}

	
	/**
	 * Checks if tpause instruction is present on current machine.
	 *
	 * @returns True if this CPU supports tpause instruction. False otherwise.
	 */
	static bool CheckForTimedPauseInstruction()
	{
#if PLATFORM_SEH_EXCEPTIONS_DISABLED
		return false;
#else
		bool bSupportsTpause = false;
		int CPUInfo[4];
		__cpuid(CPUInfo, 0);

		if (CPUInfo[0] >= 7)
		{
			int CPUExtendedInfo[4];
			__cpuidex(CPUExtendedInfo, 7, 0);

			if ((CPUExtendedInfo[2] & (1 << 5)) != 0)
			{
				// WAITPKG is supported
				try
				{
					unsigned long long tsc = __rdtsc();
					_tpause(0, tsc + 1024);
					// TPAUSE is supported
					bSupportsTpause = true;
				}
				catch (...)
				{
					if (FilterInvalidOpcode(GetExceptionCode(), GetExceptionInformation()) == EXCEPTION_EXECUTE_HANDLER)
					{
						bSupportsTpause = false;
					}
					else
					{
						throw; // Re-throw if it's not the expected exception
					}
				}
			}
		}

		return bSupportsTpause;
#endif
	}

	/**
	 * Queries Vendor name using __cpuid instruction.
	 *
	 * @returns CPU vendor name.
	 */
	static void GetCPUVendor(ANSICHAR(&OutBuffer)[12 + 1])
	{
		union
		{
			char Buffer[12 + 1];
			struct
			{
				int dw0;
				int dw1;
				int dw2;
			} Dw;
		} VendorResult;

		int Args[4];
		__cpuid(Args, 0);

		VendorResult.Dw.dw0 = Args[1];
		VendorResult.Dw.dw1 = Args[3];
		VendorResult.Dw.dw2 = Args[2];
		VendorResult.Buffer[12] = 0;

		memcpy(OutBuffer, VendorResult.Buffer,sizeof(VendorResult.Buffer));
	}

	static void GetCPUBrand(ANSICHAR(&OutBrandString)[0x40])
	{
		// @see for more information http://msdn.microsoft.com/en-us/library/vstudio/hskdteyh(v=vs.100).aspx
		ANSICHAR BrandString[0x40] = { 0 };
		Int32 CPUInfo[4] = { -1 };
		const SIZE_T CPUInfoSize = sizeof(CPUInfo);

		__cpuid(CPUInfo, 0x80000000);
		const UInt32 MaxExtIDs = CPUInfo[0];

		if (MaxExtIDs >= 0x80000004)
		{
			const UInt32 FirstBrandString = 0x80000002;
			const UInt32 NumBrandStrings = 3;
			for (UInt32 Index = 0; Index < NumBrandStrings; Index++)
			{
				__cpuid(CPUInfo, FirstBrandString + Index);
				memcpy(BrandString + CPUInfoSize * Index, CPUInfo, CPUInfoSize);
			}
		}

		memcpy(OutBrandString, BrandString, sizeof(BrandString));
	}

	/**
	 * Queries CPU info using __cpuid instruction.
	 *
	 * @returns CPU info unsigned int queried using __cpuid.
	 */
	static void QueryCPUInfo(int Args[4])
	{
		__cpuid(Args, 1);
	}

	static Int32 QueryCacheLineSize()
	{
		Int32 Result = 1;

		int Args[4];
		__cpuid(Args, 0x80000006);

		Result = Args[2] & 0xFF;

        //TODO: Add log error message
		//KASSERT(Result && !(Result & (Result - 1))); // assumed to be a power of two

		return Result;
	}
#else
	static bool CheckForCPUIDInstruction() { return false; }
	static bool CheckForTimedPauseInstruction() { return false; }
	static void GetCPUVendor(ANSICHAR(&OutBuffer)[12 + 1]) {}
	static void GetCPUBrand(ANSICHAR(&OutBrandString)[0x40]) {}
	static void QueryCPUInfo(int Args[4]) {}
	static Int32 QueryCacheLineSize() { return PLATFORM_CACHE_LINE_SIZE; }
#endif

	/** Static field with pre-cached __cpuid data. */
	static HCPUIDQueriedData CPUIDStaticCache;

	/** If machine has CPUID instruction. */
	bool bHasCPUIDInstruction;

	/** If machine has timed pause instruction. */
	bool bHasTimedPauseInstruction;

	/** Vendor of the CPU. */
	ANSICHAR Vendor[12 + 1];

	/** CPU brand. */
	ANSICHAR Brand[0x40];

	/** CPU info from __cpuid. */
	UInt32 CPUInfo;

	UInt32 CPUInfo2;

	/** CPU cache line size. */
	Int32 CacheLineSize;
};

HCPUIDQueriedData HCPUIDQueriedData::CPUIDStaticCache;

bool HWindowsPlatformMisc::HasTimedPauseCPUFeature()
{
	return HCPUIDQueriedData::HasTimedPauseInstruction();
}


