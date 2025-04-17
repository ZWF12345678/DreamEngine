#pragma once

#include "CoreType.h"
#include "Misc/EnumClassFlags.h"

#define		MAKEAFFINITYMASK1(x)							((1<<x))
#define		MAKEAFFINITYMASK2(x,y)							((1<<x)+(1<<y))
#define		MAKEAFFINITYMASK3(x,y,z)						((1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK4(w,x,y,z)						((1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK5(v,w,x,y,z)					((1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK6(u,v,w,x,y,z)					((1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK7(t,u,v,w,x,y,z)				((1<<t)+(1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK8(s,t,u,v,w,x,y,z)				((1<<s)+(1<<t)+(1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK9(r,s,t,u,v,w,x,y,z)			((1<<r)+(1<<s)+(1<<t)+(1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK10(q,r,s,t,u,v,w,x,y,z)			((1<<q)+(1<<r)+(1<<s)+(1<<t)+(1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK11(p,q,r,s,t,u,v,w,x,y,z)		((1<<p)+(1<<q)+(1<<r)+(1<<s)+(1<<t)+(1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK12(o,p,q,r,s,t,u,v,w,x,y,z)		((1<<o)+(1<<p)+(1<<q)+(1<<r)+(1<<s)+(1<<t)+(1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))
#define		MAKEAFFINITYMASK13(n,o,p,q,r,s,t,u,v,w,x,y,z)	((1<<n)+(1<<o)+(1<<p)+(1<<q)+(1<<r)+(1<<s)+(1<<t)+(1<<u)+(1<<v)+(1<<w)+(1<<x)+(1<<y)+(1<<z))


enum EThreadPriority
{
	TPri_Normal,
	TPri_AboveNormal,
	TPri_BelowNormal,
	TPri_Highest,
	TPri_Lowest,
	TPri_SlightlyBelowNormal,
	TPri_TimeCritical,
	TPri_Num,
};

enum class EThreadCreateFlags : Int8
{
	None = 0,
	SMTExclusive = (1 << 0),
};

ENUM_CLASS_FLAGS(EThreadCreateFlags);

class HUniversalPlatformAffinity
{
public:
	static const UInt64 GetMainGameMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetRenderingThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetRHIThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetRHIFrameOffsetThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetRTHeartBeatMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetPoolThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetTaskGraphThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetAudioRenderThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetNoAffinityMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetTaskGraphBackgroundTaskMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetTaskGraphHighPriorityTaskMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetAsyncLoadingThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetIoDispatcherThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const UInt64 GetTraceThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static EThreadPriority GetRenderingThreadPriority()
	{
		return TPri_Normal;
	}

	static EThreadCreateFlags GetRenderingThreadFlags()
	{
		return EThreadCreateFlags::None;
	}

	static EThreadPriority GetRHIThreadPriority()
	{
		return TPri_Normal;
	}

	static EThreadPriority GetGameThreadPriority()
	{
		return TPri_Normal;
	}

	static EThreadCreateFlags GetRHIThreadFlags()
	{
		return EThreadCreateFlags::None;
	}

	static EThreadPriority GetTaskThreadPriority()
	{
		return TPri_SlightlyBelowNormal;
	}

	static EThreadPriority GetTaskBPThreadPriority()
	{
		return TPri_BelowNormal;
	}
};