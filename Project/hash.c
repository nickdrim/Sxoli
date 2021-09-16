#include "header.h"

const float loadFactor = 0.6;
int size = 0;
int defaultSize = 30000;

int hash(char* key){
  unsigned int h=0;
	int i;

	for(i=0 ; i < strlen(key) ; i++){
		h=(h<<5 | h>>27);
		h+=(unsigned int)key[i];
	}

	return h % size;
}

HTable hashInitialize(int number){
  size = number / loadFactor;
  HTable hash = malloc(sizeof(hashtable) * size);

  for(int i=0 ; i < size ; i++){
    hash[i].start = NULL;
  }

  return hash;
}

void hashInsert(HTable records, CNode data){
  if(records == NULL) return;
  int h = hash(data->key);
  hashListInsert(&(records[h].start), data);
}

CNode hashSearch(HTable records, char* ID){
	if(records == NULL) return NULL;

	int h = hash(ID);

	if(listEmpty(records[h].start)){
		return NULL;
	}else{
		return hashListSearch(records[h].start, ID);
	}
}

void hashFree(HTable records){
  if(records == NULL) return;
  for(int i=0 ; i < size ; i++){
    if(records[i].start!=NULL){
      hashFreeList(records[i].start);
    }
  }
  free(records);
}

void hashPrint(HTable records){
  for(int i=0 ; i < size ; i++){
    if(records[i].start!=NULL){
      printf("i = %d %p",i,records[i].start);
      hashListPrint(records[i].start);
      printf("\n");
    }
    else{
      printf("i = %d %p\n",i,records[i].start);
    }
  }
}