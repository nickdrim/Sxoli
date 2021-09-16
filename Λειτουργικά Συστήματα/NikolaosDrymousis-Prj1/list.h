#ifndef LIST_H
#define LIST_H

#include "student.h"

typedef struct studentlist{
	pstudent info;
	struct studentlist* link;
}snode,*SNode;

typedef struct bucket{
	SNode inverted;
	struct bucket* chain;
}bucket,*pbucket;

typedef struct yearlist{
	int year;
	int counter;
	struct yearlist* next;
	pbucket blist;
}ynode,*YNode;

typedef struct postCode{
	int city;
	int counter;
	pbucket postlist;
	struct postCode* right;
	struct postCode* left;
}postCode,*pcode;

SNode newStudentNode(pstudent data);
void studentListInsert(SNode *head , SNode data);
void studentListDelete(SNode *head, int ID);
SNode studentListSearch(SNode head, int ID);
void studentFreeList(SNode head);

pbucket newBucketNode(SNode data);
void bucketListInsert(pbucket *head, SNode data);
void bucketListDelete(pbucket *head, SNode data);
SNode bucketListSearch(pbucket head, int ID);
void bucketFreeList(pbucket head);

YNode newYearNode(SNode data);
void yearListInsert(YNode *head , SNode data);
void yearListDelete(YNode *head, int ID, int year);
YNode yearListSearch(YNode head, int date);
void yearListSort(YNode *head);
void yearFreeList(YNode head);

pcode newPostNode(SNode data);
void postListInsert(pcode *head, SNode data);
void postListDelete(pcode *head, int ID, int city);
pcode postListSearch(pcode head, int place);
void postFreeList(pcode head);

int listEmpty(void* head);

#endif