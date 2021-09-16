#include "header.h"

int psize = 0;

//Initializes hash table for population statistics
PHash pop_hash_initialize(void){

  psize = get_hashtable_size();
  PHash temp = malloc(sizeof(pop_hash) * psize);

  for(int i = 0 ; i < psize ; i++){
    temp[i].start = NULL;
  }

  return temp;
}

//Creates a new population node
pop_ptr new_pop_node(char* item, int pid){

  pop_ptr temp = malloc(sizeof(population));
  temp->proccess_id = pid;
  temp->link = NULL;
  temp->country_name = malloc(strlen(item) + 1);
  strcpy(temp->country_name, item);

  return temp;

}

//insert item to correct hash index
void pop_hash_insert(PHash records, char* item, int pid){
  if(records == NULL) return;

  int h = hash(item,psize);

  pop_hash_list_insert(&(records[h].start), item, pid);
}

//Insert item to the population list or update statistics
void pop_hash_list_insert(pop_ptr* head, char* item, int pid){
  if(*head == NULL){
    *head = new_pop_node(item, pid);
    return;
  }

  pop_ptr traverse = (*head);

  while(traverse->link){
    traverse = traverse->link;
  }

  traverse->link = new_pop_node(item, pid);

}
 
 //Search hash to get population statistics
pop_ptr pop_hash_search(PHash records, char* item){
  if(records == NULL) return NULL;

  int h = hash(item,psize);

  pop_ptr traverse = records[h].start;

  while(traverse){
    if(!strcmp(traverse->country_name, item)){
      return traverse;
    }

    traverse = traverse->link;
  }

  return NULL;
}

void pop_hash_change(PHash records, int old, int new){
  if(records == NULL) return NULL;

  for(int i = 0 ; i < psize ; i++){
    pop_ptr traverse = records[i].start;

    while(traverse){
      if(traverse->proccess_id == old) traverse->proccess_id = new;

      traverse = traverse->link;
    }
  }

  return;
}

//Deallocate memory taken from population hash table
void pop_hash_free(PHash records){
  if(records == NULL) return;

  for(int i = 0 ; i < psize ; i++){
    if(records[i].start != NULL) pop_hash_list_free(records[i].start);
  }
  
  free(records);
}

//Deallocate memory taken from population hashlist
void pop_hash_list_free(pop_ptr head){
  if(head == NULL) return;

  pop_hash_list_free(head->link);
  free(head->country_name);
  free(head);

}