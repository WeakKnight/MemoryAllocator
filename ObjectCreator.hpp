#pragma once

#include <stdlib.h>
#include "HeapManager.hpp"

class ObjectCreator
{
public:
    template<typename T>
    static T* FCreate()
    {
        size_t size = sizeof(T);
        void* ptr = HeapManager::FGetInstance()->FAlloc(size);
        return reinterpret_cast<T*>(ptr);
    }
};
