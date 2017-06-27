#include <stdlib.h>
#include "Array.hpp"
#include "HeapManager.hpp"

template <typename T, size_t Size>
class THash
{
  public:
    unsigned long operator()(T val)
    {
        return reinterpret_cast<unsigned long>(val) % Size;
    }
};

template <typename K, typename V>
class TPair
{
  public:
    TPair(K key, V value) : MKey(key),
                            MValue(value)
    {
    }
    K MKey;
    V MValue;
};

template <typename K>
class TKeyNode
{
  public:
    TKeyNode():MNext(NULL)
    {
    }
    K MKey;
    TKeyNode<K> *MNext;
};

//based on hash table
template <typename K, typename V, size_t Size = 5000, typename H = THash<K, Size> >
class TMap
{
  public:
    TMap();
    ~TMap();
    void FPut(K key, V val);

  private:
    TArray<TPair<K, V> > MTable;
    TKeyNode<K>* MNodeList;
    H MHashFunc;
};

template <typename K, typename V, size_t Size, typename H>
void TMap<K, V, Size, H>::FPut(K key, V val)
{
    unsigned long hashValue = MHashFunc(key);
    TPair<K, V> pair(key, val);
    MTable[hashValue] = pair;


    //TODO
    if(!MNodeList)
    {
        MNodeList = new TKeyNode<K>*();
        MNodeList->MKey = key;
    }

}

template <typename K, typename V, size_t Size, typename H>
TMap<K, V, Size, H>::TMap()
{
    MNodeList = NULL;
    MTable = TArray<TPair<K, V> >(Size);
}

template <typename K, typename V, size_t Size, typename H>
TMap<K, V, Size, H>::~TMap()
{
    ~MTable();
    TKeyNode<K>* currentNode = MNodeList;
    while(currentNode)
    {
        TKeyNode<K>* nextNode = currentNode->MNext;
        delete currentNode;
        currentNode = nextNode;
    }
}