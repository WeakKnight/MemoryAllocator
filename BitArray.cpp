#include "BitArray.hpp"
#include "HeapAllocator.hpp"
#include <mm_malloc.h>
#include <assert.h>

#define BIT_PER_BYTE 8

BitArray* BitArray::FCreate(HeapAllocator* heapAllocator, size_t bitNum)
{
    BitArray* bitArray = new BitArray();
    
    bitArray->MBase = reinterpret_cast<uintptr_t*>(heapAllocator->FAlloc(bitNum/BIT_PER_BYTE));
    bitArray->MBitNum = bitNum;
    //这里面有一个假设就是bit num被8整除
    assert(bitArray->MBitNum % 8 == 0);
    return bitArray;
}

BitArray::~BitArray()
{

}

void BitArray::FClearAll()
{
    memset(MBase, 0xff, MBitNum/BIT_PER_BYTE);
}

void BitArray::FSetAll()
{
    memset(MBase, 0x00, MBitNum/BIT_PER_BYTE);
}

bool BitArray::FAreAllClear() const
{
    return true;
}

bool BitArray::FAreAllSet() const
{
    return true;
}

inline bool BitArray::FISBitSet(size_t bitNumber) const
{
    return true;
}

inline bool BitArray::FIsBitClear(size_t bitNumber) const
{
    return true;
}

void BitArray::FSetBit(size_t bitNumber)
{

}

void BitArray::FClearBit(size_t bitNumber)
{

}

bool BitArray::FGetFirstClearBit(size_t& bitNumber) const
{
    return true;
}

bool BitArray::FGetFirstSetBit(size_t& bitNumber) const
{
    return true;
}

bool BitArray::operator[](size_t index) const
{
    return true;
}