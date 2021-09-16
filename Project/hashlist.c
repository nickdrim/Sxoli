#include "header.h"

HNode newHashNode(CNode data){
  HNode temp = malloc(sizeof(hashNode));
  temp->top = data;
  temp->link = NULL;
  temp->item = malloc(strlen(data->key)+1);
  strcpy(temp->item,data->key);
  
  return temp;
}

void hashListInsert(HNode *head, CNode data){
  if(*head==NULL){
    *head = newHashNode(data);
    return;
  }

  HNode temp = newHashNode(data);

  temp->link = *head;
  *head = temp;
}

CNode hashListSearch(HNode head, char* ID){
  if(head==NULL) return NULL;

  if(!strcmp(head->item,ID)){
    return head->top;
  }else{
    HNode temp = head->link;

    while(temp!=NULL){
      if(!strcmp(temp->item,ID)) return temp->top;
      temp = temp->link;
    }
  }

  return NULL;
}

void hashFreeList(HNode head){
  if(listEmpty(head)) return;

  HNode temp = NULL;

  while(head!=NULL){
    temp = head;
    head = head->link;
    temp->link = NULL;
    temp->top = NULL;
    free(temp->item);
    free(temp);
  }
}

void hashListPrint(HNode head){
  HNode temp = head;
  while(temp!=NULL){
    printf("\t%s",temp->item);
    temp=temp->link;
  }
}

/*void hashListDelete(HNode *head, char *ID){
  if(hashListSearch(head,ID)==NULL){
    printf("Node with name %s does not exist\n",ID);
    return;
  }

  HNode temp=NULL;

  if(!strcmp((*head)->item,ID)){
    temp = *head;
    *head = (*head)->link;
    temp->link = NULL;

  }else if(!strcmp((*head)->link->item,ID)){
    temp = (*head)->link;
    (*head)->link = temp->link;
    temp->link = NULL;

  }else{
    HNode previous = (*head)->link;
    HNode traverse = previous->link;

    while(strcmp(traverse->item,ID)){
        previous = traverse;
        traverse = traverse->link;
    }

    previous->link = traverse->link;
    traverse->link = NULL;
    temp = traverse;
  }

  hashFreeList(temp);
  return;
}*/
