#pragma once

#include <stdio.h>

class BitArray
{
public:
    static BitArray* FCreate();
    ~BitArray();

    void FClearAll();
    void FSetAll();

    bool FAreAllClear() const;
    bool FAreAllSet() const;

    inline bool FISBitSet(size_t bitNumber) const;
    inline bool FIsBitClear(size_t bitNumber) const;

    void FSetBit(size_t bitNumber);
    void FClearBit(size_t bitNumber);

    bool FGetFirstClearBit(size_t& bitNumber) const;
    bool FGetFirstSetBit(size_t& bitNumber) const;

    bool operator[](size_t index) const;
};