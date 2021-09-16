#ifndef HASH_H
#define HASH_H

#include "list.h"

typedef struct hashtable{
	SNode start;
}hashtable,*HTable;


int hash(int key);
HTable hashInitialize(FILE *fp_input, FILE *fp_config, YNode *head, pcode *start);
void hashInsert(HTable records, SNode temp);
int hashDelete(HTable records, YNode *head, pcode *start, int ID);
SNode hashSearch(HTable records, int ID);
void hashFree(HTable records);

#endif