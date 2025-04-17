#pragma once

#include "Template/TRefMisc.h"

class Singleton
{
public:
    template<typename T>
    static T& GetInstance()
    {
        static T instance;
        return instance;
    }

    template<typename T, typename ... Args>
    static Ref<T> GetRefInstance(Args&& ... args)
    {
        static Ref<T> instance;
        if (instance == nullptr)
            instance = CreateRef<T>(std::forward<Args>(args)...);
        return instance;
    }

    template<typename T>
    Singleton(T&&) = delete;
    template<typename T>
    Singleton(const T&) = delete;
    template<typename T>
    void operator= (const T&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};