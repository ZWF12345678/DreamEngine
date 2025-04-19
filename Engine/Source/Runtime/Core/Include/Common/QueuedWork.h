#pragma once

#include "CoreType.h"

enum class EQueuedWorkTags
{
    None,

    DoNotRunInsideBusyWait = (1 << 0),

    Count
};

/*
* A queue thread pool executable task object
*/

class HQueuedWork
{
public:

	virtual void DoThreadedWork() = 0;

	virtual void Abandon() = 0;

	virtual EQueuedWorkTags GetQueuedWorkFlags() const { return EQueuedWorkTags::None; }
	
public:

	virtual ~HQueuedWork() {}

};