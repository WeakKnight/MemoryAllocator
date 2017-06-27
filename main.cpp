//
//  main.cpp
//  MemoryAllocator
//
//  Created by Knight on 2017/6/14.
//  Copyright © 2017年 Hermitown Studio. All rights reserved.
//

#include <iostream>
#include <mm_malloc.h>
#include "HeapAllocator.hpp"
#include "Array.hpp"
#include "HeapManager.hpp"

int main(int argc, const char * argv[]) {
    // //1mb
    // size_t heapSize = 1024 * 1024;
    // void* heapBase = malloc(heapSize);
    
    // HeapAllocator* allocator = HeapAllocator::FCreate(heapBase, heapSize, 100);
    // void* a = allocator->FAlloc(50);
    // void* b = allocator->FAlloc(150);
    // void* c = allocator->FAlloc(250);
    // void* d = allocator->FAlloc(100);
    
    // allocator->FFree(a);
    // allocator->FFree(b);
    // allocator->FFree(c);
    // allocator->FFree(d);

    // allocator->FCollect();

    // std::cout << "ptr size is "<<sizeof(uintptr_t)<<std::endl;
    // std::cout << "Largest Free Block Size is "<<allocator->FGetLargestFreeBlock()<<std::endl;
    // std::cout << "Free Size is "<<allocator->FGetFreeMemory()<<std::endl;
    HeapManager::FInitInstance();
    TArray<int> array = TArray<int>(100);
    array[50] = 20;
    std::cout<<array[50]<<std::endl;
    return 0;
}

