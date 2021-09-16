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

}

void insert(RNode *head, RNode temp){
    if(head == NULL) return;
    *head = temp;
}

void recordListEqualInsert(CNode list, pbucket head, pbucket *data,CNode cur){
  if(listEmpty(head)) return;
  CNode current = list;
  while(current!=NULL){
    if(current->clique == *data && current->key != cur->key){
        current->clique = head;
    }
    current=current->right;
  }
  
  RNode traverse = head->info;

  while(traverse->next!=NULL){
    traverse = traverse->next;
  }

  traverse->next = (*data)->info;
  head->no_items += (*data)->no_items;

  pbucket del = *data;
  *data = head;

  del->info = NULL;
  bucketFree(del);

}

RNode recordListSearch(RNode head, char* ID){
  if(head == NULL) return NULL;
  
  if(!strcmp(head->name,ID)){
    return head;
  }else{
    RNode temp = head->next;

    while(temp!=NULL){
      if(!strcmp(temp->name,ID)){
        printf("temp = %s\n",temp->name);
        return temp;
      }
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