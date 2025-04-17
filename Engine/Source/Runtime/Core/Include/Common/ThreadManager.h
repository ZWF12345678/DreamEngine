#pragma once

#include "CoreGlobal.h"

#include <unordered_map>
#include <mutex>
#include <functional>
#include <string>

class HRunnableThread;

class HThreadManager
{
    using HThreadsMap = std::unordered_map<UInt32, HRunnableThread*>;

    HThreadsMap ThreadsMap;

    std::mutex ThreadsMapMutex;

    bool IsThreadsMapDirty = false;

    bool CheckThreadsMapSafeToContinueIteration();

    void OnThreadsMapModified();

public:

    HThreadManager();

    ~HThreadManager();

    CORE_API void AddThread(UInt32 ThreadId, HRunnableThread* Thread);

    CORE_API void RemoveThread(HRunnableThread* Thread);

    UInt64 NumThreads() const { return ThreadsMap.size(); }

    //It's used when the Mode of thread is SingleThread
    CORE_API void Tick();

    inline static const std::string& GetThreadName(UInt32 ThreadId)
    {
	    static std::string GameThreadName("GameThread");
	    static std::string RenderThreadName("RenderThread");
	    if (ThreadId == GGameThreadId)
	    {
		    return GameThreadName;
	    }
	    else if (ThreadId == GRenderThreadId)
	    {
		    return RenderThreadName;
	    }
	    return Get().GetThreadNameInternal(ThreadId);
    }

    CORE_API void ForEachThread(std::function<void(UInt32 ThreadId, HRunnableThread* Thread)> Func);

    static CORE_API HThreadManager& Get();

    /** Returns internal name of a the thread given its TLS id */
    CORE_API const std::string& GetThreadNameInternal(UInt32 ThreadId);
};
