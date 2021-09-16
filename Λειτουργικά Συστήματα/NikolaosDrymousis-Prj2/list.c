#include "header.h"

void listNode(LNode head, int item, double run_time){
  memset(head,0,sizeof(listnode));
  head->number = item;
  head->item_time = run_time;
  head->next = NULL;
}

LNode createListNode(int item, double run_time){
  LNode temp = malloc(sizeof(listnode));
  temp->number = item;
  temp->item_time = run_time;
  temp->next = NULL;

  return temp;
}

void listInsertLast(LNode *head, LNode *last, int item , double run_time){
  if(*head == NULL){
    *head = createListNode(item, run_time);
    *last = *head;
    return;
  }else if((*head)->next == NULL){
    (*head)->next = createListNode(item, run_time);
    *last = (*head)->next;
    return;
  }

  LNode temp = *last;
  temp->next = createListNode(item,run_time);
  *last = temp->next;

}

void listPrint(LNode head, int lb, int ub){
  if(head==NULL){
    printf("List is empty. Nothing to print!\n");
    return;
  }

  LNode traverse = head;
  printf("Primes in [%d,%d] are: ", lb, ub);
  while(traverse!= NULL){
    printf("(%d , %f) ",traverse->number,traverse->item_time);
    traverse = traverse->next;
  }
  printf("\n");
}

void listFree(LNode head){
  if(head==NULL) return;

  LNode temp = NULL;

  while(head!=NULL){
    temp = head;
    head = head->next;
    temp->next = NULL;
    free(temp);
  }
}

