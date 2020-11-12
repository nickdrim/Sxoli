#ifndef HASH_Hhash
#define HASH_H

#include "linked_list.h"
#include "binary_tree.h"

extern int bsize;
extern int itemArraySize;
extern int sizeDisease;
extern int sizeCountries;

typedef struct bucketItem{
	char* array;
	BTNode root;
}bucketItem, *Bitem;

typedef struct bucket{
	int counter;
	Bitem* item;
	struct bucket *link;
}bucket,*pbucket;

typedef struct hashtable{
	pbucket* info;
}hashtable,*HTable;

HTable hash_initialize(int size , int BucketSize);
void hash_insert(HTable disease ,HTable country , LNode temp);
int hash(char* K , int M);
int hash_infected(HTable disease , char *target , int M);
void hash_destroy(HTable table , int total);

void destroyBucket(pbucket current);

#endif