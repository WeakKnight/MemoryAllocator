#pragma once

#include "HeapAllocator.hpp"
#include "FixedSizeAllocator.hpp"
#include <mm_malloc.h>

class HeapManager
{
  public:
    inline static void FInitInstance();
    inline static HeapManager *FGetInstance();
    void *FAlloc(size_t size);
    void *FAlloc(size_t size, unsigned int alignment);
    bool FFree(void *ptr);

  private:
    HeapAllocator *MHeapAllocator;
    FixedSizeAllocator *MFixedSizeAllocator8;
    FixedSizeAllocator *MFixedSizeAllocator16;
    FixedSizeAllocator *MFixedSizeAllocator32;
    FixedSizeAllocator *MFixedSizeAllocator64;
    FixedSizeAllocator *MFixedSizeAllocator128;
    static HeapManager *MInstance;
};

void HeapManager::FInitInstance()
{
    MInstance = new HeapManager();

    size_t heapSize = 1024 * 1024 * 150;
    void *heapBase = malloc(heapSize);

    MInstance->MHeapAllocator = HeapAllocator::FCreate(heapBase, heapSize, 5000);
    MInstance->MFixedSizeAllocator8 = FixedSizeAllocator::FCreate(MInstance->MHeapAllocator, 8, 80000);
    MInstance->MFixedSizeAllocator16 = FixedSizeAllocator::FCreate(MInstance->MHeapAllocator, 16, 40000);
    MInstance->MFixedSizeAllocator32 = FixedSizeAllocator::FCreate(MInstance->MHeapAllocator, 32, 20000);
    MInstance->MFixedSizeAllocator64 = FixedSizeAllocator::FCreate(MInstance->MHeapAllocator, 64, 10000);
    MInstance->MFixedSizeAllocator128 = FixedSizeAllocator::FCreate(MInstance->MHeapAllocator, 128, 5000);
}

HeapManager *HeapManager::FGetInstance()
{
    return MInstance;
}