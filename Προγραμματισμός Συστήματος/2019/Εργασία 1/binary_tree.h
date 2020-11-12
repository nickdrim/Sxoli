#ifndef BTTREE_H
#define BTTREE_H

#include "records.h"
#include "linked_list.h"

typedef struct tree{
	int height;
	char* date;
	rpointer data;
	struct tree *left;
	struct tree *right;
}tree,*BTNode;

typedef struct heaptree{
	int no;
	char* Name;
	struct heaptree *parent;
	struct heaptree *left;
	struct heaptree *right;
}heaptree , *HPTree;

typedef struct queueNode{
	HPTree heapNode;
	struct queueNode *next;
}queueNode ,*QNode;

typedef struct Queue{
	QNode front;
	QNode rear;
}Queue , *QHead;

int max(int a, int b);
int getBalance(BTNode node);
int height(BTNode node);
int getSize(BTNode root);
int getSizeDates(BTNode root , char *start , char *end);
int getSizeCountry(BTNode root , char *start , char *end , char* country);
int getSizeDisease(BTNode root , char *start , char *end , char* virusName);
HPTree copyNode(HPTree node);
int isHeap(HPTree heap);
int maxDepth(HPTree heap);
int checkHeap(HPTree heap);
int getSizeHeap(HPTree root);

BTNode BTinsert(BTNode root , LNode temp);
int BTinfected(BTNode root);
void BTdestroy(BTNode node);

HPTree heapInsert(HPTree node , int size , char *virusName , QHead queue);
HPTree heapify(HPTree root , int i , HPTree *temp);
int heapSize(HPTree root);
HPTree heapLevelOrder(HPTree root , int *i);
HPTree heapMax(HPTree root , int size , int counter);
void heapDestroy(HPTree heap);
void printHeap(HPTree heap);

QHead createQueue();
void deQueue(QHead q);
void enQueue(QHead q, HPTree node);
HPTree getFront(QHead q);


#endif
