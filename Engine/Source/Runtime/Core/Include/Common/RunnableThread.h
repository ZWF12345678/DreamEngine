#pragma once

#include "Common/PlatformTLS.h"
#include "Common/PlatformAffinity.h"

#include<string>

class HSyncEvent;
class HRunnable;

/*
 * An abstraction of a thread.
 *Thread needs to be implemented according to different platforms.
 */

 class HRunnableThread
{
	friend class HThreadSingletonInitializer;
	friend class HThreadManager;

	static CORE_API UInt32 RunnableTLSSlot;

public:
	/*
	 *Assign a TLSSlot to each thread.
	*/
	static CORE_API UInt32 GetTLSSlot();

	/*
	 *To create a thread, you need to call the CreateInternal function.
	*/
	static CORE_API HRunnableThread* Create(
		class HRunnable* InRunnable,
		const ANSICHAR* ThreadName,
		UInt32 InStackSize = 0,
		EThreadPriority InThreadPri = TPri_Normal,
		UInt64 InThreadAffinityMask = HPlatformAffinity::GetNoAffinityMask(),
		EThreadCreateFlags InCreateFlags = EThreadCreateFlags::None);

	/*
	 *Set the priority of the thread.
	*/
	virtual void SetThreadPriority(EThreadPriority NewPriority) = 0;

	/*
	 *Set the thread to suspend or resume.
	*/
	virtual void Suspend(bool bShouldPause = true) = 0;

	/*
	 *If bShouldWait is true, it will wait for the thread to exit before returning.
	 *Kill the thread.
	*/
	virtual bool Kill(bool bShouldWait = true) = 0;

	/*
	 *Wait for the thread to finish executing.
	*/
	virtual void WaitForCompletion() = 0;

	/*
	 *Get the thread type.
	*/
	enum class ThreadType
	{
		Real,
		Fake,
		Forkable,
	};

	virtual HRunnableThread::ThreadType GetThreadType() const
	{
		return ThreadType::Real;
	}

	const UInt32 GetThreadID() const
	{
		return ThreadID;
	}

	const std::string& GetThreadName() const
	{
		return ThreadName;
	}

	EThreadPriority GetThreadPriority() const
	{
		return ThreadPriority;
	}

	CORE_API HRunnableThread();


	CORE_API virtual ~HRunnableThread();


	static HRunnableThread* GetRunnableThread()
	{
		HRunnableThread* RunnableThread = (HRunnableThread*)HPlatformTLS::GetTLSValue(RunnableTLSSlot);
		return RunnableThread;
	}

protected:

	virtual bool CreateInternal(
		HRunnable* InRunnable, 
		const ANSICHAR* InThreadName,
		UInt32 InStackSize = 0,
		EThreadPriority InThreadPri = TPri_Normal, UInt64 InThreadAHHinityMask = 0,
		EThreadCreateFlags InCreateHlags = EThreadCreateFlags::None) = 0;

	CORE_API void SetTLS();

	CORE_API void FreeTLS();

	std::string ThreadName;

	HRunnable* Runnable;

	HSyncEvent* ThreadInitSyncEvent;

	UInt64 ThreadAffinityMask;

	EThreadPriority ThreadPriority;

	UInt32 ThreadID;

private:

	static CORE_API void SetupCreatedThread(
		HRunnableThread*& NewThread, 
		class HRunnable* InRunnable, 
		const ANSICHAR* ThreadName, 
		UInt32 InStackSize, 
		EThreadPriority InThreadPri, 
		UInt64 InThreadAffinityMask, 
		EThreadCreateFlags InCreateHlags);

	//For single-threaded applications, this is the main thread.
	virtual void Tick() {}
};