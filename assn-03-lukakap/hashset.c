#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define retBucketMicro(base, position) ((base)[(position)])

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize>0 && numBuckets>0 && hashfn != NULL && comparefn != NULL);

	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->freefn = freefn;
	h->comparefn = comparefn;
	h->hashfn = hashfn;
	h->nElem = 0;

	// initiliaze set
	h->hashSet = malloc(numBuckets*sizeof(vector*));
	for(int i = 0 ; i < numBuckets; i++){
		vector* vec = malloc(sizeof(vector));
		VectorNew(vec, elemSize, h->freefn, 0);
		retBucketMicro(h->hashSet, i) = vec;
	}
}

void HashSetDispose(hashset *h)
{
	if(h->freefn != NULL){
		for(int i = 0; i < h->numBuckets; i++){
			VectorDispose(retBucketMicro(h->hashSet,i));
			free(retBucketMicro(h->hashSet,i));
		}
	}

	free(h->hashSet);
}

int HashSetCount(const hashset *h)
{ return h->nElem; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
	for(int i = 0; i < h->numBuckets; i++){
		VectorMap(retBucketMicro(h->hashSet,i),mapfn,auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int hashCode = h->hashfn(elemAddr, h->numBuckets);
	assert(hashCode >= 0 && hashCode < h->numBuckets);

	vector* bucket = (vector*)retBucketMicro(h->hashSet, hashCode);

	int contains = VectorSearch(bucket, elemAddr, h->comparefn, 0, true);

	if(contains == -1){
		VectorAppend(bucket, elemAddr);
		VectorSort(bucket, h->comparefn);
		h->nElem++;
	} else {
		VectorReplace(bucket, elemAddr, contains);
	}
 }

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
	assert(elemAddr != NULL);
	int hashCode = h->hashfn(elemAddr, h->numBuckets);
	assert(hashCode >= 0 && hashCode < h->numBuckets);

	vector* bucket = (vector*)retBucketMicro(h->hashSet, hashCode);

	int contains = VectorSearch(bucket, elemAddr, h->comparefn, 0, true);

	if(contains == -1) return NULL;
	 return VectorNth(bucket, contains);
	}
