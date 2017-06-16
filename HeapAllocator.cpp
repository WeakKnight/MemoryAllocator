//
//  HeapAllocator.cpp
//  MemoryAllocator
//
//  Created by Knight on 2017/6/14.
//  Copyright © 2017年 Hermitown Studio. All rights reserved.
//

#include "HeapAllocator.hpp"
#include <assert.h>

void FQuickSortBlock(BlockDescriptor* start, BlockDescriptor* end)
{
    if(end != nullptr && start != end && start != end->MNext)
    {
        unsigned long x = reinterpret_cast<uintptr_t>(end->MBase);
        BlockDescriptor* i = start->MPrev;
        for(BlockDescriptor* j = start; j != end; j= j->MNext)
        {
            if(reinterpret_cast<uintptr_t>(j->MBase) <= x)
            {
                i = (i==nullptr)?start:i->MNext;
                auto temp = i->MBase;
                i->MBase = j->MBase;
                j->MBase = temp;
            }
        }
        i = (i == nullptr)? start:i->MNext;
        auto temp = i->MBase;
        i->MBase = end->MBase;
        end->MBase = temp;
        FQuickSortBlock(start, i->MPrev);
        FQuickSortBlock(i->MNext, end);
    }
}

BlockPool* BlockPool::FCreate(size_t size)
{
    auto result = new BlockPool();
    result->MSize = size;
    for (int index = 0; index < size; index++)
    {
        auto block = new BlockDescriptor();
        result->MPool.push(block);
    }
    
    return result;
}

BlockDescriptor* BlockPool::FPopBlock()
{
    //如果出现size为0的情况说明需要给HeapAllocator声明更大的block num
    assert(MPool.size() != 0);
    auto result = MPool.top();
    MPool.pop();
    return result;
}

void BlockPool::FPushBlock(BlockDescriptor* block)
{
    //clean
    block->MNext = nullptr;
    block->MPrev = nullptr;
    block->MBase = nullptr;
    block->MSize = 0;
    block->MIsFree = true;
    
    MPool.push(block);
}

HeapAllocator* HeapAllocator::FCreate(void* base, size_t size, size_t blockNum)
{
    auto result = new HeapAllocator();
    result->MBasePtr = base;
    result->MSize = size;
    
    result->MBlockPool = BlockPool::FCreate(blockNum);
    
    result->MFreeList = result->MBlockPool->FPopBlock();
    result->MFreeList->MBase = base;
    result->MFreeList->MPrev = nullptr;
    result->MFreeList->MNext = nullptr;
    result->MFreeList->MSize = size;
    result->MFreeListLast = result->MFreeList;
    
    result->MUsedList = nullptr;
    result->MUsedListLast = nullptr;
    
    result->MLargestFreeBlockSize = size;
    result->MFreeSize = size;
    
    return result;
}

void HeapAllocator::FDestroy()
{
}

void* HeapAllocator::FAlloc(size_t size)
{
    //不应该分配一块大小为0的内存
    assert(size != 0);
    //FreeList已经为空，直接返回空指针
    if(MFreeList == nullptr)
    {
        return nullptr;
    }
    
    //总可用内存不足，直接返回空指针
    if (FGetFreeMemory() < size)
    {
        return nullptr;
    }
    
    //最大块不够大，整理一次再分配
    if (FGetLargestFreeBlock() < size)
    {
        FCollect();
        if(FGetLargestFreeBlock() < size)
        {
            return nullptr;
        }
    }
    
    BlockDescriptor* currentFreeBlock = MFreeList;
    do
    {
        //如果当前的空闲Block尺寸比需要的大，但是只大了8以内，直接使用整块该Block
        if (currentFreeBlock->MSize > size && currentFreeBlock->MSize < size + 8)
        {
            MFreeSize -= currentFreeBlock->MSize;
            
            bool shouldUpdateLargestSize = false;
            if(currentFreeBlock->MSize == MLargestFreeBlockSize)
            {
                shouldUpdateLargestSize = true;
            }
            //从FreeList移除该节点,存在父节点
            if(currentFreeBlock->MPrev)
            {
                if(currentFreeBlock->MNext)
                {
                    FCombineBlock(currentFreeBlock->MPrev, currentFreeBlock->MNext);
                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if(shouldUpdateLargestSize)
                    {
                        FUpdateLargestFreeBlockSize();
                    }
                    return currentFreeBlock->MBase;
                }
                //存在父节点，不存在子节点
                else
                {
                    //从FreeList移除该节点，并且更新尾节点
                    currentFreeBlock->MPrev->MNext = nullptr;
                    MFreeListLast = currentFreeBlock->MPrev;
                    
                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if(shouldUpdateLargestSize)
                    {
                        FUpdateLargestFreeBlockSize();
                    }
                    return currentFreeBlock->MBase;
                }
            }
            //不存在父节点
            else
            {
                if(currentFreeBlock->MNext)
                {
                    MFreeList = currentFreeBlock->MNext;
                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if(shouldUpdateLargestSize)
                    {
                        FUpdateLargestFreeBlockSize();
                    }
                    return currentFreeBlock->MBase;
                }
                //也不存在子节点
                else
                {
                    MFreeList = nullptr;
                    MFreeListLast = nullptr;
                    
                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if(shouldUpdateLargestSize)
                    {
                        FUpdateLargestFreeBlockSize();
                    }
                    return currentFreeBlock->MBase;
                }
            }
        }
        //如果当前的空闲Block尺寸大于需要的很多，则进行切分
        else if (currentFreeBlock->MSize > size)
        {
            MFreeSize -= size;
            bool shouldUpdateLargestSize = false;
            if(currentFreeBlock->MSize == MLargestFreeBlockSize)
            {
                shouldUpdateLargestSize = true;
            }
            
            currentFreeBlock->MSize = currentFreeBlock->MSize - size;
            BlockDescriptor* newBlock = MBlockPool->FPopBlock();
            newBlock->MSize = size;
            newBlock->MBase = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(currentFreeBlock->MBase) + currentFreeBlock->MSize);
            FAddFreeBlockToUsedBlockList(newBlock);
            
            if(shouldUpdateLargestSize)
            {
                FUpdateLargestFreeBlockSize();
            }
            
            return newBlock->MBase;
        }
        
        currentFreeBlock = currentFreeBlock->MNext;
    }
    while (currentFreeBlock);
    
    //所有内存不足情况应该在开始就做了处理，不应该走到这里
    assert(true);
    return nullptr;
}

//TODO
void* HeapAllocator::FAlloc(size_t size, unsigned int alignment)
{
    return nullptr;
}

bool HeapAllocator::FFree(void* ptr)
{
    auto block = FFindUsedBlock(ptr);
    if(!block)
    {
        //尝试释放未通过该HeapAllocator分配的内存
        assert(true);
        return false;
    }
    
    //clean block
    //is mid node
    if(block->MPrev && block->MNext)
    {
        FCombineBlock(block->MPrev, block->MNext);
    }
    //is head
    if(!block->MPrev && block-> MNext)
    {
        MUsedList = block->MNext;
    }
    //is tail
    if(block->MPrev && !block->MNext)
    {
        MUsedListLast = block->MPrev;
    }
    //is solo
    if(!block->MPrev && !block->MNext)
    {
        MUsedListLast = nullptr;
        MUsedList = nullptr;
    }
    
    block->MPrev = nullptr;
    block->MNext = nullptr;
    
    auto currentBlock = MFreeList;
    do
    {
        if (FTryCoalesceTwoBlock(currentBlock, block))
        {
            return true;
        }
        currentBlock = currentBlock->MNext;
    }
    while (currentBlock);
    
    FAddBlockToFreeBlockList(block);
    return true;
}

void HeapAllocator::FCollect()
{
    //先快速排序
    FQuickSortBlock(MFreeList, MFreeListLast);
    auto currentFreeBlock = MFreeList;
    do
    {
        if(currentFreeBlock)
        {
            auto nextFreeBlock = currentFreeBlock->MNext;
            if(nextFreeBlock)
            {
                //如果当前空块和下一块空块相邻,则合并到下一块
                if(reinterpret_cast<uintptr_t>(currentFreeBlock->MBase) + currentFreeBlock->MSize ==  reinterpret_cast<uintptr_t>(nextFreeBlock->MBase))
                {
                    nextFreeBlock->MBase = currentFreeBlock->MBase;
                    nextFreeBlock->MSize += currentFreeBlock->MSize;
                    //
                    nextFreeBlock->MPrev = currentFreeBlock->MPrev;
                    if(currentFreeBlock->MPrev)
                    {
                        currentFreeBlock->MPrev->MNext = nextFreeBlock;
                    }
                    //回收被合并的块
                    MBlockPool->FPushBlock(currentFreeBlock);
                }
            }
            //GO Next
            currentFreeBlock = nextFreeBlock;
        }
    }
    while(currentFreeBlock);
}

bool HeapAllocator::FContain(void* ptr) const
{
    if(reinterpret_cast<uintptr_t>(ptr) >= reinterpret_cast<uintptr_t>(MBasePtr) && reinterpret_cast<uintptr_t>(ptr) <= reinterpret_cast<uintptr_t>(MBasePtr) + MSize)
    {
        return true;
    }
    return false;
}

bool HeapAllocator::FIsAllocated(void* ptr) const
{
    //如果不在堆内存中，肯定不是已分配
    if(!FContain(ptr))
    {
        return false;
    }
    
    auto currentBlock = MUsedList;
    do
    {
        if (currentBlock->MBase == ptr)
        {
            return true;
        }
        currentBlock = currentBlock->MNext;
    }
    while(currentBlock);
    
    return false;
}

size_t HeapAllocator::FGetLargestFreeBlock() const
{
    return MLargestFreeBlockSize;
}

size_t HeapAllocator::FGetFreeMemory() const
{
    return MFreeSize;
}

size_t HeapAllocator::Size() const
{
    return MSize;
}

//internal
void HeapAllocator::FCombineBlock(BlockDescriptor* a, BlockDescriptor* b)
{
    a->MNext = b;
    b->MPrev = a;
}

void HeapAllocator::FAddFreeBlockToUsedBlockList(BlockDescriptor* block)
{
    block->MPrev = nullptr;
    block->MNext = nullptr;
    //标记为已经被使用
    block->MIsFree = false;
    
    if (MUsedListLast == nullptr || MUsedList == nullptr)
    {
        assert(MUsedList == nullptr && MUsedListLast == nullptr);
        MUsedList = block;
        MUsedListLast = block;
    }
    else
    {
        FCombineBlock(MUsedListLast, block);
        MUsedListLast = block;
    }
}

void HeapAllocator::FUpdateLargestFreeBlockSize()
{
    size_t largestSize = 0;
    auto currentBlock = MFreeList;
    do
    {
        if (currentBlock->MSize > largestSize)
        {
            largestSize = currentBlock->MSize;
        }
        currentBlock = currentBlock->MNext;
    }
    while (currentBlock);
    MLargestFreeBlockSize = largestSize;
}

BlockDescriptor* HeapAllocator::FFindUsedBlock(void *ptr) const
{
    auto currentBlock = MUsedList;
    do
    {
        if(currentBlock->MBase == ptr)
        {
            return currentBlock;
        }
        currentBlock = currentBlock->MNext;
    }
    while (currentBlock);
    
    return nullptr;
}

bool HeapAllocator::FTryCoalesceTwoBlock(BlockDescriptor* freeblock, BlockDescriptor* addingBlock)
{
    if(reinterpret_cast<uintptr_t>(addingBlock->MBase) + addingBlock->MSize == reinterpret_cast<uintptr_t>(freeblock->MBase))
    {
        freeblock->MBase = addingBlock->MBase;
        freeblock->MSize += addingBlock->MSize;
        //因为这个adding block已经被合并，所以指针返回给池
        MBlockPool->FPushBlock(addingBlock);
        return true;
    }
    else if(reinterpret_cast<uintptr_t>(freeblock->MBase) + freeblock->MSize == reinterpret_cast<uintptr_t>(addingBlock->MBase))
    {
        freeblock->MSize += addingBlock->MSize;
        MBlockPool->FPushBlock(addingBlock);
        return true;
    }
    
    return false;
}

void HeapAllocator::FAddBlockToFreeBlockList(BlockDescriptor* block)
{
    //标记为可用
    block->MIsFree = true;
    if(MFreeListLast == nullptr)
    {
        assert(MFreeList == nullptr);
        MFreeList = block;
        MFreeListLast = block;
    }
    else
    {
        FCombineBlock(MFreeListLast, block);
    }
    
    if(block->MSize > MLargestFreeBlockSize)
    {
        MLargestFreeBlockSize = block->MSize;
    }
    
    MFreeSize += block->MSize;
}
