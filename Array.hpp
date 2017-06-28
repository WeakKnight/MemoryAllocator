#pragma once

#include <stdlib.h>
#include <assert.h>
#include "HeapManager.hpp"

template <typename T>
class TArray
{
  public:
    TArray(size_t capacity);
    ~TArray();
    T &operator[](size_t index);
    const T &operator[](size_t index) const;
    TArray<T> &operator=(const TArray<T> &other);

  public:
    inline bool FEmpty(const TArray<T> &a) const;
    inline void FSetSize(size_t size);
    inline size_t FGetSize() const;

  private:
    inline void FSetCapacity(size_t size);
    size_t MSize;
    size_t MCapacity;
    T *MData;
};

template <typename T>
TArray<T>::TArray(size_t capacity)
    : MCapacity(capacity),
      MSize(capacity),
      MData(NULL)
{
    MData = reinterpret_cast<T *>(HeapManager::FGetInstance()->FAlloc(capacity * sizeof(T), alignof(T)));
}

template <typename T>
TArray<T>::~TArray()
{
    HeapManager::FGetInstance()->FFree(MData);
}

template <typename T>
T &TArray<T>::operator[](size_t index)
{
    assert(index < MSize);
    return MData[index];
}

template <typename T>
const T &TArray<T>::operator[](size_t index) const
{
    assert(index < MSize);
    return MData[index];
}

template <typename T>
TArray<T> &TArray<T>::operator=(const TArray<T> &other)
{
    MSize = other.MSize;
}

template <typename T>
inline bool TArray<T>::FEmpty(const TArray<T> &a) const
{
    return (MSize == 0);
}

template <typename T>
inline void TArray<T>::FSetSize(size_t size)
{
    //if capacity is not enough
    if (size > MCapacity)
    {
        FSetCapacity(2 * size + 1);
    }
    MSize = size;
}

template <typename T>
inline void TArray<T>::FSetCapacity(size_t capacity)
{
    //change nothing, do nothing
    if (capacity == MCapacity)
    {
        return;
    }
    //capacity smaller than MCapacity, need clip MSize
    if (capacity < MCapacity)
    {
        MSize = capacity;
    }
    //update capacity
    MCapacity = capacity;
    //re alloc memory
    void *tempData = HeapManager::FGetInstance()->FAlloc(capacity * sizeof(T), alignof(T));
    memcpy(tempData, MData, capacity);
    HeapManager::FGetInstance()->FFree(MData);
    MData = reinterpret_cast<T *>(tempData);
}

template <typename T>
inline size_t TArray<T>::FGetSize() const
{
    return MSize;
}