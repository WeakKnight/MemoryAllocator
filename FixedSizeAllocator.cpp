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
    size_t bitNum;
    if (MBitArray->FGetFirstClearBit(bitNum))
    {
        MBitArray->FSetBit(bitNum);
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this->MBase) + (bitNum * MBlockSize));
    }
    
    return nullptr;
}

inline bool FixedSizeAllocator::FFree(const void* ptr)
{
    if(!FContains(ptr))
    {
        return false;
    }

    if(!FIsAllocated(ptr))
    {
        return false;
    }

    auto offset = reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(MBase);
    auto index = offset / MBlockSize;
    MBitArray->FClearBit(index);
    return true;
}

inline bool FixedSizeAllocator::FContains(const void* ptr) const
{
    int offset = reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(MBase);
    if(offset>= 0 && (offset <= MSize) && (offset % MBlockSize == 0))
    {
        return true;
    }
    return false;
}

inline bool FixedSizeAllocator::FIsAllocated(const void* ptr) const
{
    auto offset = reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(MBase);
    auto index = offset / MBlockSize;
    return MBitArray->FISBitSet(index);
}