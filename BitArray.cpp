#include "BitArray.hpp"
#include "HeapAllocator.hpp"
#include <mm_malloc.h>
#include <assert.h>

#define BIT_PER_BYTE 8

BitArray* BitArray::FCreate(HeapAllocator* heapAllocator, size_t bitCount)
{
    BitArray* bitArray = new BitArray();
    
    bitArray->MBase = reinterpret_cast<uint8_t*>(heapAllocator->FAlloc(bitCount/BIT_PER_BYTE));
    bitArray->MBitCount = bitCount;
    bitArray->MByteCount = bitCount / BIT_PER_BYTE;
    //这里面有一个假设就是bit num被8整除
    assert(bitArray->MBitCount % 8 == 0);
    //全部数值预设为0
    bitArray->FClearAll();
    //赋值heap allocator
    bitArray->MHeapAllocator = heapAllocator;
    return bitArray;
}

BitArray::~BitArray()
{
    MHeapAllocator->FFree(MBase);
}

void BitArray::FClearAll()
{
    memset(MBase, 0x00, MBitCount/BIT_PER_BYTE);
    MSetCount = 0;
}

void BitArray::FSetAll()
{
    memset(MBase, 0xff, MBitCount/BIT_PER_BYTE);
    MSetCount = MBitCount;
}

bool BitArray::FAreAllClear() const
{
    return MSetCount == 0;
}

bool BitArray::FAreAllSet() const
{
    return MSetCount == MBitCount;
}

inline bool BitArray::FISBitSet(size_t bitNumber) const
{
    int index = bitNumber /BIT_PER_BYTE;
    unsigned char offset = index % BIT_PER_BYTE;
    auto byte = MBase[index];
    
    if ((byte & (1 << (offset))) == 0x00)
    {
        return false;
    }
    return true;
}

inline bool BitArray::FIsBitClear(size_t bitNumber) const
{
    return !FISBitSet(bitNumber);
}

void BitArray::FSetBit(size_t bitNumber)
{
    if(FISBitSet(bitNumber))
    {
        return;
    }

    int index = bitNumber /BIT_PER_BYTE;
    unsigned char offset = bitNumber % BIT_PER_BYTE;
    auto byte = MBase[index];
    byte = byte | (1<< (offset));
    MBase[index] = byte;
    MSetCount += 1;
}

void BitArray::FClearBit(size_t bitNumber)
{
    if(FIsBitClear(bitNumber))
    {
        return;
    }

    int index = bitNumber /BIT_PER_BYTE;
    unsigned char offset = bitNumber % BIT_PER_BYTE;
    auto byte = MBase[index];
    MBase[index] = ~((~byte) | (1<< (offset)));
    MSetCount -= 1;
}

bool BitArray::FGetFirstClearBit(size_t& bitNumber) const
{
    size_t index = 0;
    unsigned char offset = 0;
    while(index < MByteCount && MBase[index] != 0xFF)
    {
        index++;
    }

    if(index != MByteCount)
    {
        for(int bit = 0; bit < 8; bit++)
        {
            if ((~MBase[index]) & (1<<bit))
            {
                offset = bit;
                break;
            }
        }
        bitNumber = index * 8 + offset;
        return true;
    }

    return false;
}

bool BitArray::FGetFirstSetBit(size_t& bitNumber) const
{
    size_t index = 0;
    unsigned char offset = 0;
    while(index < MByteCount && MBase[index] != 0x00)
    {
        index++;
    }

    if(index != MByteCount)
    {
        for(int bit = 0; bit < 8; bit++)
        {
            if ((MBase[index]) & (1<<bit))
            {
                offset = bit;
                break;
            }
        }
        bitNumber = index * 8 + offset;
        return true;
    }

    return false;
}

bool BitArray::operator[](size_t index) const
{
    return FISBitSet(index);
}

