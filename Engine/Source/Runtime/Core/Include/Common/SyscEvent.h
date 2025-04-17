#pragma once

#include "Misc/TimeSpan.h"
#include "Math/NumsLimits.h"


#include <memory>

/*
 *This is a conditional event that is primarily used to synchronise the order of execution between threads and the order in which events occur.
 * To reduce the performance consumption caused by frequent creation and destruction, the EventPool is usually used.
*/

class HSyncEvent
{
public:
	virtual bool Create(bool IsManualReset = false) = 0;

	virtual bool IsManualReset() const = 0;

	virtual void Trigger() = 0;

	virtual void Reset() = 0;

	virtual bool WaitFor(UInt32 WaitTime) = 0;

	bool WaitFor(const HTimespan& WaitTime, const bool IgnoreThreadIdleSate = false)
	{
        //TODO: Add log error message
		//KASSERT(WaitTime.GetTicks() >= 0);
		return WaitFor((UInt32)HMath::Clamp<Int64>(WaitTime.GetTicks() / ETimespan::TicksPerMillisecond, 0, MAX_uint32), IgnoreThreadIdleSate);
	}

	virtual bool Wait()
	{
		return WaitFor(MAX_uint32);
	}

	HSyncEvent(){}

	virtual ~HSyncEvent() {}
};

enum class ESyncEventMode { AutoReset, ManualReset };

/*
 *A conditional event reference, which automatically takes the Event from the EventPool when it is constructed and returns it when it is destroyed.
 * 
 * Note:Non-shareable
 * 
*/

class HSyncEventRef final
{
public:
	CORE_API explicit HSyncEventRef(ESyncEventMode Mode = ESyncEventMode::AutoReset);

	CORE_API ~HSyncEventRef();

	//Must use with std::move
	HSyncEventRef(const HSyncEventRef& Other)
		: Event(Other.Event)
	{}

	HSyncEventRef& operator=(const HSyncEventRef&) = delete;

	HSyncEventRef(HSyncEventRef&& Other) noexcept
		: Event(Other.Event)
	{
		Other.Event = nullptr;
	}

	HSyncEventRef& operator=(HSyncEventRef&& Other) noexcept
	{
		if (this!= &Other)
		{	
			Event = Other.Event;
			Other.Event = nullptr;
		}
		return *this;
	}

	HSyncEvent* operator->() const
	{
		return Event;
	}

	HSyncEvent* Get()
	{
		return Event;
	}

private:
	HSyncEvent* Event = nullptr;
};

/*
 * A shared conditional event reference, which automatically takes the Event from the EventPool when it is constructed and returns it when it is destroyed.
 * 
 * Note:Shareable
 * 
*/

class HSharedSyncEventRef final
{
public:
	CORE_API explicit HSharedSyncEventRef(ESyncEventMode Mode = ESyncEventMode::AutoReset);

	HSyncEvent* operator->() const
	{
		return Ptr.get();
	}

private:
	std::shared_ptr<HSyncEvent> Ptr;
};