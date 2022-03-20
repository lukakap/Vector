#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define retAddressMicro(base, position, elemSize) ((char*)(base) + ((position)*(elemSize)))


void vectorSizeIncrease(vector* v){
    v->arrayLength *= 2;
    v->array = realloc(v->array, v->arrayLength*v->elemSize);
    assert(v->array != NULL);
}


void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    assert(elemSize > 0 || initialAllocation >= 0);
     v->arrayLength = initialAllocation;
    if(initialAllocation == 0){
        v->arrayLength = 4;
    }
    v->logicLength = 0;
    v->freeFn = freeFn;
    v->array = malloc(elemSize*v->arrayLength);
    v->elemSize = elemSize;

    // if memory is overloaded
    assert(v->array != NULL);
}

void VectorDispose(vector *v)
{
    if(v->freeFn != NULL){
        for(int i = 0 ; i < v->arrayLength; i++){
           v->freeFn(retAddressMicro(v->array, i, v->elemSize));
        }
    }
}

int VectorLength(const vector *v)
{ return v->logicLength; }

void *VectorNth(const vector *v, int position)
{ 
    assert(position >= 0 && position < v->arrayLength);

    void* elemAddress = retAddressMicro(v->array, position, v->elemSize);

    return elemAddress; 
    }

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0 && position <= (v->logicLength));
    if(v->freeFn!=NULL) {
        v->freeFn(retAddressMicro(v->array, position, v->elemSize));
    }
    memcpy(retAddressMicro(v->array, position, v->elemSize), elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0 && position <= v->logicLength);
    if(v->logicLength == v->arrayLength) vectorSizeIncrease(v);

    // copy from last element to first
    for(int i = v->logicLength-1; i >= position; i--){
        memcpy(retAddressMicro(v->array, i+1, v->elemSize), retAddressMicro(v->array, i, v->elemSize), v->elemSize);
    }

    //copy new elem on position
    memcpy(retAddressMicro(v->array, position, v->elemSize), elemAddr, v->elemSize);
    v->logicLength++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    VectorInsert(v, elemAddr, v->logicLength);
}

void VectorDelete(vector *v, int position)
{
    assert(position >= 0 && position < v->logicLength);

    // free memory
    if(v->freeFn != NULL) v->freeFn(retAddressMicro(v->array, position, v->elemSize));

    // copy from first element to last
    for(int i = position; i < v->logicLength - 1; i++){
        memcpy(retAddressMicro(v->array, i, v->elemSize), retAddressMicro(v->array, i+1, v->elemSize), v->elemSize);
    }
    v->logicLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    assert(compare != NULL);
    qsort(v->array, v->logicLength, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    assert(mapFn != NULL);
    for(int i = 0 ; i < v->logicLength; i++){
      //  mapFn(retAddressMicro(v->array,i,v->elemSize), auxData);
      mapFn(VectorNth(v,i), auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
    assert(startIndex >= 0 && startIndex <= v->logicLength);
    assert(searchFn != NULL && key != NULL); 

    void* elem = NULL;
    if(isSorted){
        elem = bsearch(key, retAddressMicro(v->array, startIndex, v->elemSize), v->logicLength-startIndex, v->elemSize, searchFn);
    } else {
        size_t nmemb = v->logicLength-startIndex;
        elem = lfind(key, retAddressMicro(v->array, startIndex, v->elemSize), &nmemb, v->elemSize, searchFn);
    }

    if(elem == NULL) return kNotFound;

    return ((char*)elem - (char*)v->array)/v->elemSize;     
} 
