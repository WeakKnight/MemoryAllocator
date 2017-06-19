#include "FixedSizeAllocator.hpp"
#include "HeapAllocator.hpp"
#include "BitArray.hpp"
#include <stdlib.h>

FixedSizeAllocator* FixedSizeAllocator::FCreate(HeapAllocator* heapAllocator, size_t blockSize, size_t blockNum)
{
    //根据块数，单块尺寸初始化内存大小
    FixedSizeAllocator* allocator = new FixedSizeAllocator();
    allocator->MBlockSize = blockSize;
    allocator->MBlockNum = blockNum;
    allocator->MSize = blockSize * blockNum;
    allocator->MBase = heapAllocator->FAlloc(allocator->MSize);
    
    //初始化bitArray大小
    allocator->MBitArray = BitArray::FCreate(heapAllocator, blockNum);

    return allocator;
}

inline void* FixedSizeAllocator::FAlloc()
{
    return nullptr;
}

inline bool FixedSizeAllocator::FFree(const void *i_ptr)
{
    return true;
}

inline bool FixedSizeAllocator::FContains(const void * i_ptr) const
{
    return true;
}

inline bool FixedSizeAllocator::FIsAllocated(const void * i_ptr) const
{
    return true;
}