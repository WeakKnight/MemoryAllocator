#pragma once

#include <stdlib.h>
#include "BitArray.hpp"

class HeapAllocator;

class FixedSizeAllocator
{
public:

    static FixedSizeAllocator* FCreate(HeapAllocator* heapAllocator, size_t blockSize, size_t blockNum);
    inline void *FAlloc();
	inline bool FFree(const void* ptr);
	inline bool FContains(const void* ptr) const;
    inline bool FIsAllocated(const void* ptr) const;

private:

    size_t MBlockNum;
    size_t MBlockSize;
    size_t MSize;
    BitArray* MBitArray;
    void* MBase;
};

inline void* FixedSizeAllocator::FAlloc()
{
    size_t bitNum;
    if (MBitArray->FGetFirstClearBit(bitNum))
    {
        MBitArray->FSetBit(bitNum);
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this->MBase) + (bitNum * MBlockSize));
    }
    
    return NULL;
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

    unsigned long offset = reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(MBase);
    unsigned long index = offset / MBlockSize;
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
    unsigned long offset = reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(MBase);
    size_t index = offset / MBlockSize;
    return MBitArray->FIsBitSet(index);
}