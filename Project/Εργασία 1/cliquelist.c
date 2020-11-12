#include "header.h"

CNode newCliqueNode(char* ID){
  CNode temp = malloc(sizeof(cliqueNode));
  temp->key = malloc(strlen(ID)+1);
  strcpy(temp->key,ID);
  temp->right = NULL;
  temp->info = NULL;

  return temp;
}

void cliqueListInsert(CNode *head, HTable records, char *ID, char* location){
  if(*head==NULL){
    *head = newCliqueNode(ID);
    recordListInsert(&((*head)->info), ID, location);
    hashInsert(records, *head);
    return;
  }

  CNode temp = newCliqueNode(ID);
  recordListInsert(&(temp->info), ID, location);
  hashInsert(records, temp);

  temp->right = *head;
  *head = temp;
}

CNode cliqueListSearch(CNode head, char* ID){
  if(listEmpty(head)) return NULL;

  if(!strcmp(head->key,ID)){
    return head;
  }else{
    CNode temp = head->right;

    while(temp!=NULL){
      if(!strcmp(temp->key,ID)) return temp;
      temp = temp->right;

    }
  }

  return NULL;
}

void cliqueFreeList(CNode head){
  if(listEmpty(head)) return;

  CNode temp = NULL;

  while(head!=NULL){
    temp = head;
    head = head->right;
    CNode traverse = temp->right;
    while(traverse!=NULL){
      if(traverse->info == temp->info){
        traverse->info = NULL;
      }
      traverse = traverse->right;
    }
    if(temp->info!=NULL ) recordFreeList(temp->info);
    free(temp->key);
    free(temp);
  }
}

void cliqueListPrintOriginal(CNode head){
  if(listEmpty(head)) return;

  CNode temp = head;

  while(temp!=NULL){
    printf("%s\t",temp->key);
    recordListPrint(temp->info);
    printf("\n");
    temp=temp->right;
  }
}

/*void cliqueListDelete(CNode *head, char *ID){
  if(cliqueListSearch(head,ID)==NULL){
    printf("Node with name %s does not exist\n",ID);
    return;
  }

  CNode temp=NULL;

  if(!strcmp((*head)->key,ID)){
    temp = *head;
    *head = (*head)->right;
    temp->right = NULL;

  }else if(!strcmp((*head)->right->key,ID)){
    temp = (*head)->right;
    (*head)->right = temp->right;
    temp->right = NULL;

  }else{
    CNode previous = (*head)->right;
    CNode traverse = previous->right;

    while(strcmp(traverse->key,ID)){
        previous = traverse;
        traverse = traverse->right;
    }

    previous->right = traverse->right;
    traverse->right = NULL;
    temp = traverse;
  }

  cliqueFreeList(temp);
  return;

}*/