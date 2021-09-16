#ifndef LIST_H
#define LIST_H

typedef struct list{
  int number;
  double item_time;
	struct list* next;
}listnode,*LNode;

LNode createListNode(int item , double run_time);
void listInsertLast(LNode *head, LNode *last, int item, double run_time);
void listPrint(LNode head , int lb , int ub);
void listFree(LNode head);
void listNode(LNode head, int item, double run_time);

#endif