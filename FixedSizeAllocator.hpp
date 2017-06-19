#pragma once

#include <stdio.h>

class HeapAllocator;
class BitArray;

class FixedSizeAllocator
{
public:

    static FixedSizeAllocator* FCreate(HeapAllocator* heapAllocator, size_t blockSize, size_t blockNum);
    inline void *FAlloc();
	inline bool FFree(const void *i_ptr);
	inline bool FContains(const void * i_ptr) const;
    inline bool FIsAllocated(const void * i_ptr) const;

private:

    size_t MBlockNum;
    size_t MBlockSize;
    size_t MSize;
    BitArray* MBitArray;
    void* MBase;
};