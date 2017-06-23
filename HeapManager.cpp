#include "HeapManager.hpp"
#include <stdlib.h>

HeapManager* HeapManager::MInstance = NULL;

void* HeapManager::FAlloc(size_t size)
{
    if(size <= 8)
    {
        return MFixedSizeAllocator8->FAlloc();
    }
    if(size <= 16)
    {
        return MFixedSizeAllocator16->FAlloc();
    }
    if(size <= 32)
    {
        return MFixedSizeAllocator32->FAlloc();
    }
    if(size <= 64)
    {
        return MFixedSizeAllocator64->FAlloc();
    }
    if(size <= 128)
    {
        return MFixedSizeAllocator128->FAlloc();
    }

    return MHeapAllocator->FAlloc(size);
}

void* HeapManager::FAlloc(size_t size, unsigned int alignment)
{
    return MHeapAllocator->FAlloc(size, alignment);
}

bool HeapManager::FFree(void* ptr)
{
    if (MFixedSizeAllocator8->FContains(ptr))
    {
        return MFixedSizeAllocator8->FFree(ptr);
    }
    if (MFixedSizeAllocator16->FContains(ptr))
    {
        return MFixedSizeAllocator16->FFree(ptr);
    }
    if (MFixedSizeAllocator32->FContains(ptr))
    {
        return MFixedSizeAllocator32->FFree(ptr);
    }
    if (MFixedSizeAllocator64->FContains(ptr))
    {
        return MFixedSizeAllocator64->FFree(ptr);
    }
    if (MFixedSizeAllocator128->FContains(ptr))
    {
        return MFixedSizeAllocator128->FFree(ptr);
    }
    if (MHeapAllocator->FContains(ptr))
    {
        return MHeapAllocator->FFree(ptr);
    }
    return false;
}