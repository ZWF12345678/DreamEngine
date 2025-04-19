#pragma once

#include "CoreType.h"

#include <string>
#include <string.h>


class HMallocator;


/** implementation for most platforms, these tend to be unused and unimplemented. */
struct HUniversalPlatformMemory
{
    enum EMemoryAllocatorType
    {
        StandardC
    };

    /** Current allocator */
	static CORE_API EMemoryAllocatorType CurrentAllocatorType;

    enum ESharedMemoryAccess
	{
		Read	=		(1 << 1),
		Write	=		(1 << 2)
	};

    /**
	 * Generic representation of a shared memory region
	 */
    struct SSharedMemoryRegion
    {

        SSharedMemoryRegion(const ANSICHAR* InName, UInt32 InAccessMode, void* InAddress, SIZE_T InSize);

        SSharedMemoryRegion(const std::string& InName, UInt32 InAccessMode, void* InAddress, SIZE_T InSize);

        const ANSICHAR* GetName() const
        {
            return Name;
        }

        void* GetAddress()
        {
            return Address;
        }

        const void* GetAddress() const
        {
            return Address;
        }

        SIZE_T GetSize() const
        {
            return Size;
        }

    protected:

        enum NameLength
        {
            MaxNameLength = 128
        };

        ANSICHAR    Name[MaxNameLength];	// Name of the shared memory region

        UInt32      AccessMode;

        void*       Address;	// Address of the shared memory region

        SIZE_T      Size;		// Size of the shared memory region

    };


    static CORE_API void Init();

    [[noreturn]] static CORE_API void OnOutOfMemory(UInt64 Size, UInt32 Alignment);

    /** Initializes the memory pools, should be called by the init function. */

    static CORE_API void InitMemoryPools();

    static UInt32 GetBackMemoryPoolSize()
	{
		// by default, don't create a backup memory buffer
		return 0;
	}

    /**
	 * @return the default allocator.
	 */
    static CORE_API HMallocator* GetDefaultAllocator();

};


