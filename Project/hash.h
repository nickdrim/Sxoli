#ifndef HASH_H
#define HASH_H

#include "list.h"

typedef struct hashtable{
  HNode start;
}hashtable,*HTable;

int hash(char* key);
HTable hashInitialize(int number);
void hashInsert(HTable records, CNode data);
//int hashDelete(HTable records, char* ID);
CNode hashSearch(HTable records, char* ID);
void hashFree(HTable records);
void hashPrint(HTable records);

#endif