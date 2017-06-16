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

int main(int argc, const char * argv[]) {
    // insert code here...
    size_t heapSize = 1024 * 1024;
    void* heapBase = malloc(heapSize);
    
    auto allocator = HeapAllocator::FCreate(heapBase, heapSize, 100);
    
    auto a = allocator->FAlloc(50);
    auto b = allocator->FAlloc(150);
    auto c = allocator->FAlloc(250);
    auto d = allocator->FAlloc(100);
    
    allocator->FFree(a);
    allocator->FFree(b);
    allocator->FFree(c);
    allocator->FFree(d);

    allocator->FCollect();

    std::cout << "Largest Free Block Size is"<<allocator->FGetLargestFreeBlock()<<std::endl;
    std::cout << "Free Size is"<<allocator->FGetFreeMemory()<<std::endl;
    return 0;
}

