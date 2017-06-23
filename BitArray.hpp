#pragma once

#include <stdlib.h>

#define BIT_PER_BYTE 8

class HeapAllocator;

class BitArray
{
public:
    static BitArray* FCreate(HeapAllocator* heapAllocator, size_t bitCount);
    ~BitArray();

    void FClearAll();
    void FSetAll();

    bool FAreAllClear() const;
    bool FAreAllSet() const;

    inline bool FIsBitSet(size_t bitNumber) const;
    inline bool FIsBitClear(size_t bitNumber) const;

    void FSetBit(size_t bitNumber);
    void FClearBit(size_t bitNumber);

    bool FGetFirstClearBit(size_t& bitNumber) const;
    bool FGetFirstSetBit(size_t& bitNumber) const;

    bool operator[](size_t index) const;

private:
    uint8_t* MBase;
    size_t MBitCount;
    size_t MByteCount;
    size_t MSetCount;
    HeapAllocator* MHeapAllocator;
};

inline bool BitArray::FIsBitSet(size_t bitNumber) const
{
    int index = bitNumber /BIT_PER_BYTE;
    unsigned char offset = index % BIT_PER_BYTE;
    uint8_t byte = MBase[index];
    
    if ((byte & (1 << (offset))) == 0x00)
    {
        return false;
    }
    return true;
}

inline bool BitArray::FIsBitClear(size_t bitNumber) const
{
    int index = bitNumber /BIT_PER_BYTE;
    unsigned char offset = index % BIT_PER_BYTE;
    uint8_t byte = MBase[index];
    
    if ((byte & (1 << (offset))) == 0x00)
    {
        return true;
    }
    return false;
}