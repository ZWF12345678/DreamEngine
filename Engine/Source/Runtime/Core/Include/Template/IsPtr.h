#pragma once

template <typename T>
struct TIsPtr
{
    enum { Value = false };
};

template <typename T>
struct TIsPtr<T*>
{
    enum { Value = true };
};

template <typename T> 
struct TIsPtr<const T> 
{ 
    enum { Value = TIsPtr<T>::Value }; 
};

template <typename T> 
struct TIsPtr<volatile T> 
{ 
    enum { Value = TIsPtr<T>::Value }; 
};

template <typename T> 
struct TIsPtr<const volatile T> 
{ 
    enum { Value = TIsPtr<T>::Value }; 
};
