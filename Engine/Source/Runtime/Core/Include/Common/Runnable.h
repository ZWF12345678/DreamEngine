#pragma once

#include "CoreType.h"

/*
 * A execution body that can run on a thread
*/

class  HRunnable
{
public:

	virtual bool Init()
	{
		return true;
	}

	virtual UInt32 Run() = 0;

	virtual void Stop() {}

	virtual void Exit() {}

	virtual ~HRunnable() {}
};