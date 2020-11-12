#ifndef LIST_H
#define LIST_H

#include "hash.h"

typedef struct recordlist{
  char* name;
  char* path;
  struct recordlist *next;
}recordNode,*RNode;


typedef struct cliqueList{
  char* key;
  struct cliqueList *right;
  RNode info;
}cliqueNode,*CNode;

typedef struct hashList{
  char* item;
  struct hashList* link;
  CNode top;
}hashNode,*HNode;


RNode newRecordNode(char* ID, char* location);
void recordListInsert(RNode *head, char* ID, char* location);
//void recordListDelete(RNode *head, char* ID);
RNode recordListSearch(RNode head, char* ID);
void recordFreeList(RNode head);
void recordListEqualInsert(CNode list, RNode head, RNode *data, CNode cur);
void recordListPrint(RNode head);

CNode newCliqueNode(char* ID);
//void cliqueListDelete(CNode *head, char* ID);
CNode cliqueListSearch(CNode head, char* ID);
void cliqueFreeList(CNode head);
void cliqueListPrintOriginal(CNode head);

HNode newHashNode(CNode data);
void hashListInsert(HNode *head, CNode data);
//void hashListDelete(HNode *head, char* ID);
CNode hashListSearch(HNode head, char* ID);
void hashFreeList(HNode head);
void hashListPrint(HNode head);

void insert(RNode *head, RNode temp);

int listEmpty(void *head);

#endif