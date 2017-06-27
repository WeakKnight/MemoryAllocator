#include <stdlib.h>
#include "HeapManager.hpp"

template<typename T>
class TArray
{
public:
    TArray(size_t capacity);
    ~TArray();
    T& operator[](size_t index);
	const T& operator[](size_t index) const;
	TArray<T>& operator=(const TArray<T>& other);

public:
    inline bool FEmpty(const TArray<T>& a);
    inline void FSetSize(size_t size);
    inline size_t FGetSize();

private:
    inline void FSetCapacity(size_t size);
    size_t MSize;
    size_t MCapacity;
    T* MData;
};

template<typename T>
TArray<T>::TArray(size_t capacity)
:
MCapacity(capacity),
MSize(0),
MData(NULL)
{
    MData = reinterpret_cast<T*>(HeapManager::FGetInstance()->FAlloc(capacity * sizeof(T), alignof(T)));
}

template<typename T>
TArray<T>::~TArray()
{
    HeapManager::FGetInstance()->FFree(MData);
}

template<typename T>
T& TArray<T>::operator[](size_t index)
{
    assert(index < MSize);
    return MData[index];
}

template<typename T>
const T& TArray<T>::operator[](size_t index) const
{
    assert(index < MSize);
    return MData[index];
}

template<typename T>
TArray<T>& TArray<T>::operator=(const TArray<T>& other)
{
    MSize = other.MSize;
}

template<typename T>
inline bool TArray<T>::FEmpty(const TArray<T>& a)
{
    return (MSize == 0);
}

template<typename T>
inline void TArray<T>::FSetSize(size_t size)
{
    //if capacity is not enough
    if(size > MCapacity)
    {
        FSetCapacity(2 * size + 1);
    }
    MSize = size;
}

template<typename T>
inline void TArray<T>::FSetCapacity(size_t capacity)
{
    //change nothing, do nothing
    if(capacity == MCapacity)
    {
        return;
    }
    //capacity smaller than MCapacity, need clip MSize
    if(capacity < MCapacity)
    {
        MSize = capacity;
    }
    //update capacity
    MCapacity = capacity;
    //re alloc memory
    void* tempData = HeapManager::FGetInstance()->FAlloc(capacity * sizeof(T), alignof(T));
    memset(MData, tempData, capacity);
    HeapManager::FGetInstance()->FFree(MData);
    MData = reinterpret_cast<T*>(tempData);
}

template<typename T>
inline size_t TArray<T>::FGetSize()
{
    return MSize;
}