#ifndef LIST_H
#define LIST_H

#include "records.h"

typedef struct list{
	rpointer record;
	struct list* next;
}nodes,*LNode;

LNode list_insert(LNode tail , LNode node);
LNode newNode(char **arr ,int counter);
LNode list_search(LNode *head , int x);
void list_destroy(LNode head);
void printList(LNode head);


#endif