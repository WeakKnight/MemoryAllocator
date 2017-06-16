//
//  HeapAllocator.hpp
//  MemoryAllocator
//
//  Created by Knight on 2017/6/14.
//  Copyright © 2017年 Hermitown Studio. All rights reserved.
//
#pragma once

#include <stdio.h>
#include <stack>

struct BlockDescriptor
{
    BlockDescriptor* MPrev;
    BlockDescriptor* MNext;
    size_t MSize;
    void* MBase;
    bool MIsFree;
    BlockDescriptor():
    MPrev(nullptr),
    MNext(nullptr),
    MSize(0),
    MBase(nullptr),
    MIsFree(true)
    {
        
    }
};

class BlockPool
{
public:
    static BlockPool* FCreate(size_t size);
    BlockDescriptor* FPopBlock();
    void FPushBlock(BlockDescriptor* block);
    
private:
    std::stack<BlockDescriptor*> MPool;
    size_t MSize;
};

class HeapAllocator
{
public:
    static HeapAllocator* FCreate(void* base, size_t size, size_t blockNum);
    void FDestroy();
    void* FAlloc(size_t size);
    void* FAlloc(size_t size, unsigned int alignment);
    bool FFree(void* ptr);
    void FCollect();
    bool FContain(void* ptr) const;
    bool FIsAllocated(void* ptr) const;
    size_t FGetLargestFreeBlock() const;
    size_t FGetFreeMemory() const;
    size_t Size() const;
    
    BlockDescriptor* MFreeList;
    BlockDescriptor* MFreeListLast;
    BlockDescriptor* MUsedList;
    BlockDescriptor* MUsedListLast;
    
private:
    BlockPool* MBlockPool;
    void FCombineBlock(BlockDescriptor* a, BlockDescriptor* b);
    void FAddFreeBlockToUsedBlockList(BlockDescriptor* block);
    void FUpdateLargestFreeBlockSize();
    BlockDescriptor* FFindUsedBlock(void* ptr) const;
    bool FTryCoalesceTwoBlock(BlockDescriptor* freeblock, BlockDescriptor* addingBlock);
    void FAddBlockToFreeBlockList(BlockDescriptor* block);
    
private:
    //HEAP起点的内存地址
    void* MBasePtr;
    size_t MSize;
    size_t MFreeSize;
    size_t MLargestFreeBlockSize;
};

