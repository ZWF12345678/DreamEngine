#pragma once

#include "CoreType.h"
#include <cstddef> // For size_t

enum
{
    DEFAULT_ALIGMENT = 0
};

/** The global memory allocator. */
CORE_API extern class HMallocator* GMallocator;

class HSystemMemoryMallocator
{
public:

    CORE_API void* operator new(size_t Size);

    CORE_API void operator delete(void* Ptr);

    void* operator new[](size_t Size);

    void operator delete[](void* Ptr);
};

/** The global memory allocator's interface. */

class HMallocator : public HSystemMemoryMallocator
{
public:

    virtual void* Malloc(SIZE_T Size, UInt32 Alignment = DEFAULT_ALIGMENT) = 0;

    CORE_API virtual void* TryMalloc(SIZE_T Size, UInt32 Alignment = DEFAULT_ALIGMENT);

    virtual void* Realloc(void* Ptr, SIZE_T Size, UInt32 Alignment = DEFAULT_ALIGMENT) = 0;

    CORE_API virtual void* TryRealloc(void* Ptr, SIZE_T Size, UInt32 Alignment = DEFAULT_ALIGMENT);

    virtual void Free(void* Ptr) = 0;

    CORE_API virtual void* MallocZeroed(SIZE_T Size, UInt32 Alignment = DEFAULT_ALIGMENT);

    CORE_API virtual void* TryMallocZeroed(SIZE_T Size, UInt32 Alignment = DEFAULT_ALIGMENT);

    virtual SIZE_T QuantizeSize(SIZE_T Count, UInt32 Alignment)
	{
		return Count; // Default implementation has no way of determining this
	}

    virtual bool GetAllocationSize(void *Original, SIZE_T &SizeOut)
	{
		return false; // Default implementation has no way of determining this
	}

    virtual void Trim(bool bTrimThreadCaches)
	{
	}

    virtual void SetupTLSCachesOnCurrentThread()
	{
	}

    virtual void MarkTLSCachesAsUsedOnCurrentThread()
	{
	}

    virtual void MarkTLSCachesAsUnusedOnCurrentThread()
	{
	}

    virtual void ClearAndDisableTLSCachesOnCurrentThread()
	{
	}

    virtual bool IsInternallyThreadSafe() const 
	{ 
		return false; 
	}

    virtual void OnMallocInitialized() {}

    virtual UInt64 GetFreeCachedMemorySize() const
	{
		return 0;
	}

};