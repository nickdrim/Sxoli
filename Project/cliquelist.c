#include "header.h"

int clique_unique_ids = 0;

pbucket newBucket(void){
  pbucket temp = malloc(sizeof(bucket));
  temp->info = NULL;
  temp->id = clique_unique_ids;
  clique_unique_ids++;
  temp->negative_relationships = 0;
  temp->arr = NULL;
  temp->arr_size = 0;
  temp->no_items = 0;

  return temp;
}

void bucketFree(pbucket head){
  if(head == NULL) return;

  if(head->info != NULL) recordFreeList(head->info);
  free(head->arr);
  free(head);

  return;
}

CNode newCliqueNode(char* ID){
  CNode temp = malloc(sizeof(cliqueNode));
  temp->key = malloc(strlen(ID)+1);
  strcpy(temp->key,ID);
  temp->right = NULL;
  temp->clique = newBucket();

  return temp;
}

void cliqueListInsert(CNode *head, HTable records, char *ID, char* location){
  if(hashSearch(records, ID) != NULL){
    return;
  }

  if(*head==NULL){
    *head = newCliqueNode(ID);
    (*head)->clique->no_items++;
    recordListInsert(&((*head)->clique->info), ID, location);
    hashInsert(records, *head);
    return;
  }

  CNode temp = newCliqueNode(ID);
  temp->clique->no_items++;
  recordListInsert(&(temp->clique->info), ID, location);
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
      if(traverse->clique == temp->clique){
        traverse->clique = NULL;
      }
      traverse = traverse->right;
    }
    if(temp->clique != NULL ) bucketFree(temp->clique);
    free(temp->key);
    free(temp);
  }
}

void cliqueListPrintOriginal(CNode head){
  if(listEmpty(head)) return;

  CNode temp = head;

  while(temp!=NULL){
    printf("%s\t",temp->key);
    recordListPrint(temp->clique->info);
    printf("\n");
    temp=temp->right;
  }
}