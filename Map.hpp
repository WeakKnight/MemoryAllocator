#pragma once

#include <stdlib.h>
#include "Array.hpp"
#include "HeapManager.hpp"
#include "ObjectCreator.hpp"

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
    unsigned long MHashValue;
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
    V FGet(K key);

  private:
    void FInternalInsertNode(unsigned long hashValue);
    bool FInternalSearchNode();

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
    
    FInternalInsertNode(hashValue);
}

template <typename K, typename V, size_t Size, typename H>
V TMap<K, V, Size, H>::FGet(K key)
{
    unsigned long hashValue = MHashFunc(key);
    TKeyNode<K>* currentNode = MNodeList;
    TKeyNode<K>* preNode = NULL;
    while(currentNode)
    {
        //found
        if(currentNode->MHashValue == hashValue)
        {
            return MTable[hashValue].MValue;
        }
        preNode = currentNode;
        currentNode = currentNode->MNext;
    }
    //not found
    return NULL;
}

template <typename K, typename V, size_t Size, typename H>
void TMap<K, V, Size, H>::FInternalInsertNode(unsigned long hashValue)
{
    if(MNodeList)
    {
        //loop node list and check if exists in list 
        TKeyNode<K>* currentNode = MNodeList;
        TKeyNode<K>* preNode = NULL;
        while(currentNode)
        {
            if(currentNode->MHashValue == hashValue)
            {
                return;
            }
            preNode = currentNode;
            currentNode = currentNode->MNext;
        }
        //link into last Node
        TKeyNode<K>* node = ObjectCreator::FCreate<TKeyNode<K> >();
        //TKeyNode<K>* node = new TKeyNode<K>();
        node->MNext = NULL;
        node->MHashValue = hashValue;
        preNode->MNext = node;
    }
    //if node list is null, create node list
    else if(!MNodeList)
    {
        //link into last Node
        TKeyNode<K>* node = ObjectCreator::FCreate<TKeyNode<K> >();
        //TKeyNode<K>* node = new TKeyNode<K>();
        node->MNext = NULL;
        node->MHashValue = hashValue;
        MNodeList = node;
    }
}

template <typename K, typename V, size_t Size, typename H>
TMap<K, V, Size, H>::TMap()
:MTable(TArray<TPair<K, V> >(Size))
{
    MNodeList = NULL;
}

template <typename K, typename V, size_t Size, typename H>
TMap<K, V, Size, H>::~TMap()
{
    TKeyNode<K>* currentNode = MNodeList;
    while(currentNode)
    {
        TKeyNode<K>* nextNode = currentNode->MNext;
        delete currentNode;
        currentNode = nextNode;
    }
}