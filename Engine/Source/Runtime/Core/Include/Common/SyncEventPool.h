#pragma once

#include "Common/SyscEvent.h"
#include "Common/PlatformProcess.h"
#include "Container/AtomicMpMcQueue.h"

class HSafeRecycleableSyncEvent final :public HSyncEvent
{
public:
    HSyncEvent* InnnerEvent;

	HSafeRecycleableSyncEvent(HSyncEvent* innerEvent)
		:InnnerEvent(innerEvent)
	{
	}

	~HSafeRecycleableSyncEvent()
	{
		InnnerEvent = nullptr;
	}

	virtual bool Create(bool IsManualReset = false)
	{
		return InnnerEvent->Create(IsManualReset);
	}

	virtual bool IsManualReset() const
	{
		return InnnerEvent->IsManualReset();
	}

	virtual void Trigger()
	{
		InnnerEvent->Trigger();
	}

	virtual void Reset()
	{
		InnnerEvent->Reset();
	}

	virtual bool WaitFor(UInt32 WaiteTime)
	{
		return InnnerEvent->WaitFor(WaiteTime);
	}

	virtual bool Wait()
	{
		return InnnerEvent->Wait();
	}
};

template<ESyncEventMode PoolType>
class HSyncEventPool
{
public:
	~HSyncEventPool()
	{
		EmptyPool();
	}

	HSyncEvent* GetSyncEventFromPool()
	{
		return new HSafeRecycleableSyncEvent(GetRawSyncEvent());
	}

	void ReturnToPool(HSyncEvent* Event)
	{
		if (Event)
		{
            //TODO: Add log error message
			//KASSERT(Event->IsManualReset() == (PoolType == ESyncEventMode::ManualReset));

			HSafeRecycleableSyncEvent* SafeEvent = (HSafeRecycleableSyncEvent*)(Event);
			ReturnRawSyncEvent(SafeEvent->InnnerEvent);
			delete SafeEvent;
		}
	}

	void EmptyPool()
	{
		HSyncEvent* Event = nullptr;
		while (EventsPool.try_dequeue(Event))
		{
			delete Event;
		}
	}

public:
	HSyncEvent* GetRawSyncEvent()
	{
		HSyncEvent* Event = nullptr;
		if (!EventsPool.try_dequeue(Event))
		{
			Event = HPlatformProcess::CreateSynchEvent(PoolType == ESyncEventMode::ManualReset);
		}
		return Event;
	}

	void ReturnRawSyncEvent(HSyncEvent* Event)
	{
		if (Event)
		{
			Event->Reset();
			bool Success = EventsPool.enqueue(Event);
            //TODO: Add log error message
			//KASSERT(Success);
		}
	}
private:
	moodycamel::AtomicMpMcQueue<HSyncEvent*> EventsPool;
};
