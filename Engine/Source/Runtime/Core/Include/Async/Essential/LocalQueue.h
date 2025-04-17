#pragma once

#include "CoreType.h"
#include "Common/PlatformTime.h"
#include "Container/AtomicMpMcQueue.h"

#include <atomic>

#define LOCALQUEUEREGISTRYDEFAULTS_MAX_ITEMCOUNT	1024
#define LOCALQUEUEREGISTRYDEFAULTS_MAX_LOCALQUEUES	1024

class HTask;

/*
 *This is a task queue slot that can be stolen from, and other threads can steal tasks from the queue via TLocalQueueRegistry.
 * A slot has two states: Free, meaning it is not occupied and new tasks can be placed in that slot; Taken, indicating that the task in that slot is currently being stolen.
 * NOTE: The Put and Get methods are not thread-safe operations and therefore are only suitable for use as a LocalQueue operated by a single thread; the Steal method is thread-safe.
*/

namespace TaskOfTaskGraph
{
    namespace LocalQueueImpl
    {
        template<UInt32 NumItems>
        class TWorkStealingQueueBase
        {
            enum class ESlotState : uintptr_t
            {
                Free = 0,					//can be used
                Taken = 1,					//is taken by another thread, can't use
            };

        protected:
            // (this can only safe on a single thread) 
            inline bool Put(uintptr_t InItem)
            {
                //TODO: Add log error message
                // KASSERT(InItem != uintptr_t(ESlotState::Free));
                // KASSERT(InItem != uintptr_t(ESlotState::Taken));

                UInt32 Index = (Head + 1) % NumItems;
                uintptr_t OldValue = Slots[Index].Value.load(std::memory_order_acquire);

                if(OldValue == uintptr_t(ESlotState::Free))
                {
                    Slots[Index].Value.store(InItem, std::memory_order_release);
                    Head ++;
                    //KASSERT(Head % NumItems == Index);
                    return true;
                }
                return false;
            }

            // FIFO order (this can only safe on a single thread)
            inline bool Get(uintptr_t& OutItem)
            {
                UInt32 Index = Head % NumItems;
                uintptr_t OldValue = Slots[Index].Value.load(std::memory_order_acquire);

                if(OldValue > uintptr_t(ESlotState::Taken) 
                && Slots[Index].Value.compare_exchange_strong(OldValue, uintptr_t(ESlotState::Free),std::memory_order_acq_rel))
                {
                    Head--;
                    //TODO: Add log error message
                    //KASSERT((Head + 1) % NumItems == Index);
                    OutItem = OldValue;
                    return true;
                }
                return false;
            }

            //Steal it from the position of the tail, and get it in the order of LIFO.
            inline boo steal(uintptr_t& OutItem)
            {
                do
                {
                    UInt32 OldIndexVersion  = Tail.load(std::memory_order_acquire);
                    UInt32 OldIndex = (OldIndexVersion) % NumItems;
                    uintptr_t OldValue = Slots[OldIndex].Value.load(std::memory_order_acquire);

                    if(OldValue == uintptr_t(ESlotState::Free))
                    {
                        if(OldIndexVersion != Tail.load(std::memory_order_acquire))
                        {
                            continue;
                        }
                        return false;
                    }
                    else if (OldValue != uintptr_t(ESlotState::Taken)
                        && Slots[OldIndex].Value.compare_exchange_weak(OldValue, uintptr_t(ESlotState::Taken),std::memory_order_acq_rel))
                    {
                        if(OldIndexVersion == Tail.load(std::memory_order_acquire))
                        {
                            UInt32 PreValue = Tail.fetch_add(1,std::memory_order_release);
                            (void) PreValue;
                            //TODO: Add log error message
                            //KASSERT(PreValue % NumItems == Idx);
                            Slots[OldIndex].Value.store(uintptr_t(ESlotState::Free),std::memory_order_release);
                            OutItem = OldValue;
                            return true;
                        }
                        Slots[OldIndex].Value.store(OldValue, std::memory_order_release);
                    }
                } while (true);      
            }
            
        private:
            struct SAlignedElement
            {
                alignas(PLATFORM_CACHE_LINE_SIZE * 2) std::atomic<uintptr_t> Value = {};
            };

            alignas(PLATFORM_CACHE_LINE_SIZE * 2) UInt32 Head { ~0u };
            
            alignas(PLATFORM_CACHE_LINE_SIZE * 2) std::atomic_uint Tail { 0 };

            alignas(PLATFORM_CACHE_LINE_SIZE * 2) SAlignedElement Slots[NumItems] = {};

        };

        /*store the task pointer in the queue, and use the state to mark it as taken or free*/
        template<typename Type, UInt32 NumItems>
        class TWorkStealingQueue final : protected TWorkStealingQueueBase<NumItems>
        {
            using PointerType = Type*;

        public:
            inline bool Put(PointerType InItem)
            {
                return TWorkStealingQueueBase<NumItems>::Put(reinterpret_cast<uintptr_t>(InItem));
            }

            inline bool Get(PointerType& OutItem)
            {
                return TWorkStealingQueueBase<NumItems>::Get(reinterpret_cast<uintptr_t&>(OutItem));
            }

            inline bool Steal(PointerType& OutItem)
            {
                return TWorkStealingQueueBase<NumItems>::Steal(reinterpret_cast<uintptr_t&>(OutItem));
            }

        };
    }

    enum class ELocalQueueType
    {
        Background,
        Foreground,
    };

    template<UInt32 NumLocalItems = LOCALQUEUEREGISTRYDEFAULTS_MAX_ITEMCOUNT, UInt32 MaxLocalQueues = LOCALQUEUEREGISTRYDEFAULTS_MAX_LOCALQUEUES>
    class TLocalQueueRegister
    {
        static UInt32 Rand()
        {
            UInt32 State = HPlatformTime::Cycles();
            State = State * 747796405u + 2891336453u;
            State = ((State >> ((State >> 28u) + 4u)) ^ State) * 277803737u;
            return (State >> 22u) ^ State;
        }

    public:
	    class TLocalQueue;

    private:
         using HLocalQueue = TWorkStealingQueue<HTask, NumLocalItems>;
         using HOverflowQueue = moodycamel::AtomicMpMcQueue<HTask*>;

    public:
        class TLocalQueue
        {
            template<UInt32,UInt32>
            friend class TLocalQueueRegister;

        public:
            TLocalQueue(TLocalQueueRegister& InRegister, ELocalQueueType InQueueType)
                :Register(InRegister),
                QueueType(InQueueType)
            {
                Register->AddLocalQueue(this);
            }

            ~TLocalQueue()
            {
                for (Int32 PriorityIndex = 0; PriorityIndex < Int32(ETaskPriority::Count); PriorityIndex++)
                {
                    while (true)
                    {
                        HTask* Item;
                        if (!LocalQueues[PriorityIndex].Get(Item))
                        {
                            break;
                        }
                        if (!Register->OverflowQueues[PriorityIndex].try_enqueue(Item))
                        {
                            //TODO
                            // K_LOG_ERROR(LOGTO_CONSOLE, "Failed to enqueue task to global queue,task will be abandoned");
                            // delete Item;
                        }
                    }
                }
                Registry = nullptr;
            }

            inline void Enqueue(HTask* Item, UInt32 PriorityIndex)
            {
                //TODO
                // KASSERT(Registry);
                // KASSERT(PriorityIndex < Int32(ETaskPriority::Count));
                // KASSERT(Item != nullptr);

                if (!LocalQueues[PriorityIndex].Put(Item))
                {
                    if (!Registry->OverflowQueues[PriorityIndex].try_enqueue(Item))
                    {
                        //TODO
                        // K_LOG_ERROR(LOGTO_CONSOLE, "Failed to enqueue task ,task will be abandoned");
                        // delete Item;
                    }
                }
            }

            // Check both the local and global queue in priority order
            inline HTask* Dequeue(bool GetBackGroundTasks)
            {
                const Int32 MaxPriority = GetBackGroundTasks ? Int32(ETaskPriority::Count) : Int32(ETaskPriority::ForegroundCount);
                HTask* Item = nullptr;
                for (Int32 PriorityIndex = 0; PriorityIndex < MaxPriority; ++PriorityIndex)
                {
                    if (LocalQueues[PriorityIndex].Get(Item))
                    {
                        return Item;
                    }

                    Registry->OverflowQueues[PriorityIndex].try_dequeue(Item);
                    if (Item)
                    {
                        return Item;
                    }
                }
                return Item;
            }

            inline HTask* DequeueSteal(bool GetBackGroundTasks)
            {
                if (CachedRandomIndex == InvalidIndex)
                {
                    CachedRandomIndex = Rand();
                }

                HTask* Result = Registry->StealItem(CachedRandomIndex, CachedPriorityIndex, GetBackGroundTasks);
                if (Result)
                {
                    return Result;
                }
                return nullptr;
            }
        
        private:
            static constexpr UInt32     InvalidIndex = ~0u;
            HLocalQueue                 LocalQueues[UInt32(ETaskPriority::Count)];
            TLocalQueueRegister*        Register = nullptr;
            UInt32						CachedRandomIndex = InvalidIndex;
            UInt32						CachedPriorityIndex = 0;
            ELocalQueueType				QueueType;
        };

        TLocalQueueRegistry()
        {
        }

    private:
        // Add a queue to the Registry. Thread-safe.
        void AddLocalQueue(TLocalQueue* QueueToAdd)
        {
            UInt32 Index = NumLocalQueues.fetch_add(1, std::memory_order_relaxed);
            if (Index >= MaxLocalQueues)
            {
                //TODO
                //K_LOG_ERROR(LOGTO_CONSOLE, "Attempting to add more than the maximum allowed number of queues {0}", MaxLocalQueues);
            }
            LocalQueues[Index].store(QueueToAdd, std::memory_order_release);
        }

        // StealItem tries to steal an Item from a Registered LocalQueue,Thread-safe with AddLocalQueue.
        HTask* StealItem(UInt32& CachedRandomIndex, UInt32& CachedPriorityIndex, bool GetBackGroundTasks)
        {
            UInt32 NumQueues = NumLocalQueues.load(std::memory_order_relaxed);
            UInt32 MaxPriority = GetBackGroundTasks ? Int32(ETaskPriority::Count) : Int32(ETaskPriority::ForegroundCount);
            CachedRandomIndex = CachedRandomIndex % NumQueues;

            for (UInt32 Index = 0; Index < NumLocalQueues; Index++)
            {
                if (TLocalQueue* LocalQueue = LocalQueues[Index].load(std::memory_order_acquire))
                {
                    for (UInt32 PriorityIndex = 0; PriorityIndex < MaxPriority; PriorityIndex++)
                    {
                        HTask* Item;
                        if (LocalQueue->LocalQueues[PriorityIndex].Steal(Item))
                        {
                            return Item;
                        }
                        CachedPriorityIndex = ++CachedPriorityIndex < MaxPriority ? CachedPriorityIndex : 0;
                    }
                    CachedRandomIndex = ++CachedRandomIndex < NumQueues ? CachedRandomIndex : 0;
                }
            }
            /*not steal from other local queue*/
            CachedPriorityIndex = 0;
            CachedRandomIndex = TLocalQueue::InvalidIndex;
            return nullptr;
        }

    public:
        // enqueue an Item directy into the Global OverQueues
        void Enqueue(HTask* Item, UInt32 PriorityIndex)
        {
            //TODO
            // KASSERT(PriorityIndex < Int32(ETaskPriority::Count));
            // KASSERT(Item != nullptr);

            if (!OverflowQueues[PriorityIndex].try_enqueue(Item))
            {
                // K_LOG_ERROR(LOGTO_CONSOLE, "Failed to enqueue task to global queue,task will be abandoned");
                // delete Item;
            }
        }

        // grab an Item directy from the Global OverflowQueue
        HTask* DequeueGlobal(bool GetBackGroundTasks = true)
        {
            const Int32 MaxPriority = GetBackGroundTasks ? Int32(ETaskPriority::Count) : Int32(ETaskPriority::ForegroundCount);

            HTask* Item = nullptr;
            for (Int32 PriorityIndex = 0; PriorityIndex < MaxPriority; ++PriorityIndex)
            {
                if (OverflowQueues[PriorityIndex].try_dequeue(Item))
                {
                    return Item;
                }
            }
            return Item;
        }

        inline HTask* DequeueSteal(bool GetBackGroundTasks)
        {
            UInt32 CachedRandomIndex = Rand();
            UInt32 CachedPriorityIndex = 0;
            HTask* Result = StealItem(CachedRandomIndex, CachedPriorityIndex, GetBackGroundTasks);
            if (Result)
            {
                return Result;
            }
            return nullptr;
        }

        void Reset()
        {
            UInt32 NumQueues = NumLocalQueues.load(std::memory_order_relaxed);
            for (UInt32 Index = 0; Index < NumQueues; Index++)
            {
                LocalQueues[Index].store(0, std::memory_order_relaxed);
            }

            NumLocalQueues.store(0, std::memory_order_release);
        }

    private:
        HOverflowQueue                  OverflowQueues[UInt32(ETaskPriority::Count)];
        std::atomic<TLocalQueue*>       LocalQueues[MaxLocalQueues] {nullptr};
        std::atomic<UInt32>             NumLocalQueues {0};
    };
}

