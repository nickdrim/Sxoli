#ifndef LIST_H
#define LIST_H

#include "hash.h"

typedef struct recordlist{
  char* name;
  char* path;
  struct recordlist *next;
}recordNode,*RNode;

typedef struct bucket{
  int id;
  int no_items;
  struct bucket **arr;
  int arr_size;
  RNode info;
  int negative_relationships;
}bucket, *pbucket;


typedef struct cliqueList{
  char* key;
  struct cliqueList *right;
  pbucket clique;
}cliqueNode,*CNode;

typedef struct hashList{
  char* item;
  struct hashList* link;
  CNode top;
}hashNode,*HNode;


RNode newRecordNode(char* ID, char* location);
void recordListInsert(RNode *head, char* ID, char* location);
RNode recordListSearch(RNode head, char* ID);
void recordFreeList(RNode head);
void recordListEqualInsert(CNode list, pbucket head, pbucket *data, CNode cur);
void recordListPrint(RNode head);

CNode newCliqueNode(char* ID);
CNode cliqueListSearch(CNode head, char* ID);
void cliqueFreeList(CNode head);
void cliqueListPrintOriginal(CNode head);

HNode newHashNode(CNode data);
void hashListInsert(HNode *head, CNode data);
CNode hashListSearch(HNode head, char* ID);
void hashFreeList(HNode head);
void hashListPrint(HNode head);

pbucket newBucket(void);
void bucketFree(pbucket head);

void insert(RNode *head, RNode temp);

int listEmpty(void *head);

#endif