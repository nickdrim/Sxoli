#include "header.h"

int listEmpty(void* head){
	if(head == NULL){
		//printf("List is empty\n");
		return 1;
	}

	return 0;
}

RNode newRecordNode(char* ID, char* location){
  RNode temp = malloc(sizeof(recordNode));
  temp->name = malloc(strlen(ID)+1);
  strcpy(temp->name,ID);
  temp->path = malloc(strlen(location)+1);
  strcpy(temp->path,location);
  temp->next = NULL;

  return temp;
}

void recordListInsert(RNode *head, char* ID, char* location){
  if(*head==NULL){
    *head = newRecordNode(ID, location);
    return;
  }

  /*RNode temp = newRecordNode(ID, location);
  printf("I am free\n");
  
  temp->next = *head;
  *head = temp;*/

}

void insert(RNode *head, RNode temp){
  *head = temp;
}

void recordListEqualInsert(CNode list, RNode head, RNode *data,CNode cur){
  if(listEmpty(head)) return;

  RNode traverse = head;
  while(traverse->next != NULL){
    traverse = traverse->next;
  }

  CNode current = list;
  while(current!=NULL){
    RNode temp = *data;
    if(current->info == *data && current->key != cur->key){
        insert(&current->info , head);
    }
    current=current->right;
  }
  
  traverse->next = *data;
  *data = head;
}

RNode recordListSearch(RNode head, char* ID){
  if(listEmpty(head)) return NULL;

  if(!strcmp(head->name,ID)){
    return head;
  }else{
    RNode temp = head->next;

    while(temp!=NULL){
      if(!strcmp(temp->name,ID)) return temp;
      temp=temp->next;

    }
  }

  return NULL;
}

void recordFreeList(RNode head){
  if(listEmpty(head)) return;

  RNode temp = NULL;

  while(head!=NULL){
    temp = head;
    head = head->next;
    temp->next = NULL;
    free(temp->name);
    free(temp->path);
    free(temp);
  }
}

void recordListPrint(RNode head){
  if(listEmpty(head)) return;

  RNode temp = head;
  while(temp!=NULL){
    printf("%s\t",temp->name);
    temp = temp->next;
  }
}

/*void recordListDelete(RNode *head, char* ID){

  if(recordListSearch(head,ID)==NULL){
    printf("Node with name %s does not exists\n",ID);
    return;
  }

  RNode temp=NULL;

  if(!strcmp((*head)->name,ID)){
    temp = *head;
    *head = (*head)->next;
    temp->next = NULL;

  }else if(!strcmp((*head)->next->name,ID)){
    temp = (*head)->next;
    (*head)->next = temp->next;
    temp->next = NULL;

  }else{
    RNode previous = (*head)->next;
    RNode traverse = previous->next;

    while(strcmp(traverse->name,ID)){
        previous = traverse;
        traverse = traverse->next;
    }

    previous->next = traverse->next;
    traverse->next = NULL;
    temp = traverse;
  }

  recordFreeList(temp);
  return;
}*/