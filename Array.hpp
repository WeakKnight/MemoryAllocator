#include <stdlib.h>
#include "HeapManager.hpp"

template<typename T>
class Array
{
public:
    Array(size_t capacity);
    ~Array();
    T& operator[](size_t index);
	const T& operator[](size_t index) const;
	Array<T>& operator=(const Array<T>& other);
public:
    inline bool FEmpty(const Array<T>& a);
    inline void FResize(size_t size);

private:
    inline void FSetCapacity(size_t size);
    size_t MSize;
    size_t MCapacity;
    T* MData;
};

template<typename T>
Array<T>::Array(size_t capacity)
:
MCapacity(capacity),
MSize(0),
MData(NULL)
{
    MData = HeapManager::FGetInstance()->FAlloc(capacity * sizeof(T), alignof(T));
}

template<typename T>
Array<T>::~Array()
{
    HeapManager::FGetInstance()->FFree(MData);
}

template<typename T>
T& Array<T>::operator[](size_t index)
{
    return MData[index];
}

template<typename T>
const T& Array<T>::operator[](size_t index) const
{
    return MData[index];
}

template<typename T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
    MSize = other.MSize;
}

template<typename T>
inline bool Array<T>::FEmpty(const Array<T>& a)
{
    return (MSize == 0);
}

template<typename T>
inline void Array<T>::FResize(size_t size)
{

}

template<typename T>
inline void Array<T>::FSetCapacity(size_t size)
{
    
}