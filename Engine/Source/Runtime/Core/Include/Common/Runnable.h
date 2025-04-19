#pragma once

#include "CoreType.h"

/*
 * A execution body that can run on a thread
*/

class  HRunnable
{
public:
	/*
	 *You can add some process to the method that needs to be initialised, and there must be a return value, 
	 *when false is returned, it means that the initialisation has failed and will not run.
	*/
	virtual bool Init()
	{
		return true;
	}

	/*
	 *The method that is called when the thread is started, and the return value is the exit code of the thread.
	 *If you want to do something, you should add it here.
	*/
	virtual UInt32 Run() = 0;

	/*
	 *The method that is called when the thread is stopped
	*/
	virtual void Stop() {}

	/*
	 *After the execution is complete, there are some aftereffects that you need to address and you should include them here.
	 *The method that is called when the thread is exited
	*/
	virtual void Exit() {}

	virtual ~HRunnable() {}
};