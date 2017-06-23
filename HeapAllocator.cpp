//
//  HeapAllocator.cpp
//  MemoryAllocator
//
//  Created by Knight on 2017/6/14.
//  Copyright © 2017年 Hermitown Studio. All rights reserved.
//

#include "HeapAllocator.hpp"
#include <assert.h>

void FQuickSortBlock(BlockDescriptor *start, BlockDescriptor *end)
{
    if (end != NULL && start != end && start != end->MNext)
    {
        unsigned long x = reinterpret_cast<uintptr_t>(end->MBase);
        BlockDescriptor *i = start->MPrev;
        for (BlockDescriptor *j = start; j != end; j = j->MNext)
        {
            if (reinterpret_cast<uintptr_t>(j->MBase) <= x)
            {
                i = (i == NULL) ? start : i->MNext;
                void *tempBase = i->MBase;
                size_t tempSize = i->MSize;
                i->MBase = j->MBase;
                i->MSize = j->MSize;
                j->MBase = tempBase;
                j->MSize = tempSize;
            }
        }
        i = (i == NULL) ? start : i->MNext;
        void *tempBase = i->MBase;
        size_t tempSize = i->MSize;
        i->MBase = end->MBase;
        i->MSize = end->MSize;
        end->MBase = tempBase;
        end->MSize = tempSize;
        FQuickSortBlock(start, i->MPrev);
        FQuickSortBlock(i->MNext, end);
    }
}

BlockPool *BlockPool::FCreate(size_t size)
{
    BlockPool *result = new BlockPool();
    result->MSize = size;
    for (int index = 0; index < size; index++)
    {
        BlockDescriptor *block = new BlockDescriptor();
        //result->MPool.push(block);
        if (!result->MPoolList)
        {
            result->MPoolList = block;
            result->MPoolListLast = block;
        }
        else
        {
            result->MPoolListLast->MNext = block;
            block->MPrev = result->MPoolListLast;
            result->MPoolListLast = block;
        }
    }

    return result;
}

BlockDescriptor *BlockPool::FPopBlock()
{
    //如果出现size为0的情况说明需要给HeapAllocator声明更大的block num
    assert(MPoolListLast != NULL);
    BlockDescriptor *result = MPoolListLast;
    //update Pool List
    MPoolListLast = MPoolListLast->MPrev;
    MPoolListLast->MNext = NULL;
    if (MPoolListLast == NULL)
    {
        MPoolList = NULL;
    }
    //clean result
    result->MPrev = NULL;
    result->MNext = NULL;

    return result;
}

void BlockPool::FPushBlock(BlockDescriptor *block)
{
    //clean
    block->MNext = NULL;
    block->MPrev = NULL;
    block->MBase = NULL;
    block->MSize = 0;
    block->MIsFree = true;

    if (MPoolList == NULL)
    {
        MPoolList = block;
        MPoolListLast = block;
    }
    else
    {
        MPoolListLast->MNext = block;
        block->MPrev = MPoolListLast;
        MPoolListLast = block;
    }

    //MPool.push(block);
}

HeapAllocator *HeapAllocator::FCreate(void *base, size_t size, size_t blockNum)
{
    HeapAllocator *result = new HeapAllocator();
    result->MBasePtr = base;
    result->MSize = size;

    result->MBlockPool = BlockPool::FCreate(blockNum);

    result->MFreeList = result->MBlockPool->FPopBlock();
    result->MFreeList->MBase = base;
    result->MFreeList->MPrev = NULL;
    result->MFreeList->MNext = NULL;
    result->MFreeList->MSize = size;
    result->MFreeListLast = result->MFreeList;

    result->MUsedList = NULL;
    result->MUsedListLast = NULL;

    result->MLargestFreeBlockSize = size;
    result->MFreeSize = size;

    return result;
}

void HeapAllocator::FDestroy()
{
}

void *HeapAllocator::FAlloc(size_t size)
{
    //不应该分配一块大小为0的内存
    assert(size != 0);
    //FreeList已经为空，直接返回空指针
    if (MFreeList == NULL)
    {
        return NULL;
    }

    //总可用内存不足，直接返回空指针
    if (FGetFreeMemory() < size)
    {
        return NULL;
    }

    //最大块不够大，整理一次再分配
    if (FGetLargestFreeBlock() < size)
    {
        FCollect();
        if (FGetLargestFreeBlock() < size)
        {
            return NULL;
        }
    }

    BlockDescriptor *currentFreeBlock = MFreeList;
    do
    {
        //如果当前的空闲Block尺寸比需要的大，但是只大了8以内，直接使用整块该Block
        if (currentFreeBlock->MSize > size && currentFreeBlock->MSize < size + 8)
        {
            MFreeSize -= currentFreeBlock->MSize;

            bool shouldUpdateLargestSize = false;
            if (currentFreeBlock->MSize == MLargestFreeBlockSize)
            {
                shouldUpdateLargestSize = true;
            }
            //从FreeList移除该节点,存在父节点
            if (currentFreeBlock->MPrev)
            {
                if (currentFreeBlock->MNext)
                {
                    FCombineBlock(currentFreeBlock->MPrev, currentFreeBlock->MNext);
                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if (shouldUpdateLargestSize)
                    {
                        FUpdateLargestFreeBlockSize();
                    }
                    return currentFreeBlock->MBase;
                }
                //存在父节点，不存在子节点
                else
                {
                    //从FreeList移除该节点，并且更新尾节点
                    currentFreeBlock->MPrev->MNext = NULL;
                    MFreeListLast = currentFreeBlock->MPrev;

                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if (shouldUpdateLargestSize)
                    {
                        FUpdateLargestFreeBlockSize();
                    }
                    return currentFreeBlock->MBase;
                }
            }
            //不存在父节点
            else
            {
                if (currentFreeBlock->MNext)
                {
                    MFreeList = currentFreeBlock->MNext;
                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if (shouldUpdateLargestSize)
                    {
                        FUpdateLargestFreeBlockSize();
                    }
                    return currentFreeBlock->MBase;
                }
                //也不存在子节点
                else
                {
                    MFreeList = NULL;
                    MFreeListLast = NULL;

                    FAddFreeBlockToUsedBlockList(currentFreeBlock);
                    if (shouldUpdateLargestSize)
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
            if (currentFreeBlock->MSize == MLargestFreeBlockSize)
            {
                shouldUpdateLargestSize = true;
            }

            currentFreeBlock->MSize = currentFreeBlock->MSize - size;
            BlockDescriptor *newBlock = MBlockPool->FPopBlock();
            newBlock->MSize = size;
            newBlock->MBase = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(currentFreeBlock->MBase) + currentFreeBlock->MSize);
            FAddFreeBlockToUsedBlockList(newBlock);

            if (shouldUpdateLargestSize)
            {
                FUpdateLargestFreeBlockSize();
            }

            return newBlock->MBase;
        }

        currentFreeBlock = currentFreeBlock->MNext;
    } while (currentFreeBlock);

    //所有内存不足情况应该在开始就做了处理，不应该走到这里
    assert(true);
    return NULL;
}

void *HeapAllocator::FAlloc(size_t size, unsigned int alignment)
{
    //不应该分配一块大小为0的内存
    assert(size != 0);
    //FreeList已经为空，直接返回空指针
    if (MFreeList == NULL)
    {
        return NULL;
    }

    //总可用内存不足，直接返回空指针
    if (FGetFreeMemory() < size)
    {
        return NULL;
    }

    //最大块不够大，整理一次再分配
    if (FGetLargestFreeBlock() < size)
    {
        FCollect();
        if (FGetLargestFreeBlock() < size)
        {
            return NULL;
        }
    }

    BlockDescriptor *currentFreeBlock = MFreeList;
    do
    {
        unsigned long extraSize = (reinterpret_cast<uintptr_t>(currentFreeBlock->MBase) + currentFreeBlock->MSize - size) % alignment;
        if (currentFreeBlock->MSize > size + extraSize)
        {
            MFreeSize -= size;
            bool shouldUpdateLargestSize = false;
            if (currentFreeBlock->MSize == MLargestFreeBlockSize)
            {
                shouldUpdateLargestSize = true;
            }

            currentFreeBlock->MSize = currentFreeBlock->MSize - size;
            BlockDescriptor *newBlock = MBlockPool->FPopBlock();
            newBlock->MSize = size;
            newBlock->MBase = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(currentFreeBlock->MBase) + currentFreeBlock->MSize - extraSize);
            newBlock->MSize -= extraSize;

            currentFreeBlock->MSize += extraSize;

            FAddFreeBlockToUsedBlockList(newBlock);

            if (shouldUpdateLargestSize)
            {
                FUpdateLargestFreeBlockSize();
            }

            return newBlock->MBase;
        }

        currentFreeBlock = currentFreeBlock->MNext;
    } while (currentFreeBlock);

    //所有内存不足情况应该在开始就做了处理，不应该走到这里
    assert(true);
    return NULL;
}

bool HeapAllocator::FFree(void *ptr)
{
    BlockDescriptor *block = FFindUsedBlock(ptr);
    if (!block)
    {
        //尝试释放未通过该HeapAllocator分配的内存
        assert(true);
        return false;
    }

    //clean block
    //is mid node
    if (block->MPrev && block->MNext)
    {
        FCombineBlock(block->MPrev, block->MNext);
    }
    //is head
    if (!block->MPrev && block->MNext)
    {
        MUsedList = block->MNext;
        block->MNext->MPrev = NULL;
    }
    //is tail
    if (block->MPrev && !block->MNext)
    {
        MUsedListLast = block->MPrev;
        block->MPrev->MNext = NULL;
    }
    //is solo
    if (!block->MPrev && !block->MNext)
    {
        MUsedListLast = NULL;
        MUsedList = NULL;
    }

    block->MPrev = NULL;
    block->MNext = NULL;

    // auto currentBlock = MFreeList;
    // do
    // {
    //     if (FTryCoalesceTwoBlock(currentBlock, block))
    //     {
    //         return true;
    //     }
    //     currentBlock = currentBlock->MNext;
    // }
    // while (currentBlock);

    FAddBlockToFreeBlockList(block);
    return true;
}

void HeapAllocator::FCollect()
{
    //先快速排序
    FQuickSortBlock(MFreeList, MFreeListLast);
    BlockDescriptor *currentFreeBlock = MFreeList;
    do
    {
        if (currentFreeBlock)
        {
            BlockDescriptor *nextFreeBlock = currentFreeBlock->MNext;
            if (nextFreeBlock)
            {
                //如果当前空块和下一块空块相邻,则合并到下一块
                if (reinterpret_cast<uintptr_t>(currentFreeBlock->MBase) + currentFreeBlock->MSize == reinterpret_cast<uintptr_t>(nextFreeBlock->MBase))
                {
                    nextFreeBlock->MBase = currentFreeBlock->MBase;
                    nextFreeBlock->MSize += currentFreeBlock->MSize;
                    //
                    nextFreeBlock->MPrev = currentFreeBlock->MPrev;
                    if (currentFreeBlock->MPrev)
                    {
                        currentFreeBlock->MPrev->MNext = nextFreeBlock;
                    }
                    if (currentFreeBlock == MFreeList)
                    {
                        MFreeList = nextFreeBlock;
                    }
                    //回收被合并的块
                    MBlockPool->FPushBlock(currentFreeBlock);
                }
            }
            //GO Next
            currentFreeBlock = nextFreeBlock;
        }
    } while (currentFreeBlock);
    FUpdateLargestFreeBlockSize();
}

bool HeapAllocator::FContains(void *ptr) const
{
    if (reinterpret_cast<uintptr_t>(ptr) >= reinterpret_cast<uintptr_t>(MBasePtr) && reinterpret_cast<uintptr_t>(ptr) <= reinterpret_cast<uintptr_t>(MBasePtr) + MSize)
    {
        return true;
    }
    return false;
}

bool HeapAllocator::FIsAllocated(void *ptr) const
{
    //如果不在堆内存中，肯定不是已分配
    if (!FContains(ptr))
    {
        return false;
    }

    BlockDescriptor *currentBlock = MUsedList;
    do
    {
        if (currentBlock->MBase == ptr)
        {
            return true;
        }
        currentBlock = currentBlock->MNext;
    } while (currentBlock);

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
void HeapAllocator::FCombineBlock(BlockDescriptor *a, BlockDescriptor *b)
{
    a->MNext = b;
    b->MPrev = a;
}

void HeapAllocator::FAddFreeBlockToUsedBlockList(BlockDescriptor *block)
{
    block->MPrev = NULL;
    block->MNext = NULL;
    //标记为已经被使用
    block->MIsFree = false;

    if (MUsedListLast == NULL || MUsedList == NULL)
    {
        assert(MUsedList == NULL && MUsedListLast == NULL);
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
    BlockDescriptor *currentBlock = MFreeList;
    do
    {
        if (currentBlock->MSize > largestSize)
        {
            largestSize = currentBlock->MSize;
        }
        currentBlock = currentBlock->MNext;
    } while (currentBlock);
    MLargestFreeBlockSize = largestSize;
}

BlockDescriptor *HeapAllocator::FFindUsedBlock(void *ptr) const
{
    if (!MUsedList)
    {
        return NULL;
    }

    BlockDescriptor *currentBlock = MUsedList;
    do
    {
        if (currentBlock->MBase == ptr)
        {
            return currentBlock;
        }
        currentBlock = currentBlock->MNext;
    } while (currentBlock);

    return NULL;
}

bool HeapAllocator::FTryCoalesceTwoBlock(BlockDescriptor *freeblock, BlockDescriptor *addingBlock)
{
    if (reinterpret_cast<uintptr_t>(addingBlock->MBase) + addingBlock->MSize == reinterpret_cast<uintptr_t>(freeblock->MBase))
    {
        freeblock->MBase = addingBlock->MBase;
        freeblock->MSize += addingBlock->MSize;
        //因为这个adding block已经被合并，所以指针返回给池
        MBlockPool->FPushBlock(addingBlock);
        return true;
    }
    else if (reinterpret_cast<uintptr_t>(freeblock->MBase) + freeblock->MSize == reinterpret_cast<uintptr_t>(addingBlock->MBase))
    {
        freeblock->MSize += addingBlock->MSize;
        MBlockPool->FPushBlock(addingBlock);
        return true;
    }

    return false;
}

void HeapAllocator::FAddBlockToFreeBlockList(BlockDescriptor *block)
{
    //标记为可用
    block->MIsFree = true;
    if (MFreeListLast == NULL)
    {
        assert(MFreeList == NULL);
        MFreeList = block;
        MFreeListLast = block;
    }
    else
    {
        FCombineBlock(MFreeListLast, block);
        MFreeListLast = block;
    }

    if (block->MSize > MLargestFreeBlockSize)
    {
        MLargestFreeBlockSize = block->MSize;
    }

    MFreeSize += block->MSize;
}
