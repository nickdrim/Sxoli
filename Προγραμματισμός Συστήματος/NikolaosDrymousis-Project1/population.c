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
pop_ptr new_pop_node(char* item, int age){

  pop_ptr temp = malloc(sizeof(population));
  temp->population = 1;
  temp->link = NULL;
  temp->country_name = malloc(strlen(item) + 1);
  strcpy(temp->country_name, item);

  if(age <= 20){
    temp->pop_total20 = 1;
    temp->pop_total40 = 0;
    temp->pop_total60 = 0;
    temp->pop_total60plus = 0;
  }else if(age <= 40){
    temp->pop_total20 = 0;
    temp->pop_total40 = 1;
    temp->pop_total60 = 0;
    temp->pop_total60plus = 0;
  }else if(age <= 60){
    temp->pop_total20 = 0;
    temp->pop_total40 = 0;
    temp->pop_total60 = 1;
    temp->pop_total60plus = 0;
  }else{
    temp->pop_total20 = 0;
    temp->pop_total40 = 0;
    temp->pop_total60 = 0;
    temp->pop_total60plus = 1;
  }

  return temp;

}

//insert item to correct hash index
void pop_hash_insert(PHash records, char* item, int age){
  if(records == NULL) return;

  int h = hash(item);

  pop_hash_list_insert(&(records[h].start), item, age);
}

//Insert item to the population list or update statistics
void pop_hash_list_insert(pop_ptr* head, char* item, int age){
  if(*head == NULL){
    *head = new_pop_node(item, age);
    return;
  }else if(!strcmp((*head)->country_name,item)){
    (*head)->population++;

    if(age <= 20){
      (*head)->pop_total20++;
    }else if(age <= 40){
      (*head)->pop_total40++;
    }else if(age <= 60){
      (*head)->pop_total60++;
    }else{
      (*head)->pop_total60plus++;
    }

    return;
  }

  pop_ptr traverse = (*head);

  while(traverse->link){
    if(!strcmp(traverse->link->country_name, item)){
      traverse->link->population++;
      if(age <= 20){
        traverse->pop_total20++;
      }else if(age <= 40){
        traverse->pop_total40++;
      }else if(age <= 60){
        traverse->pop_total60++;
      }else{
        traverse->pop_total60plus++;
      }
      return;
    }

    traverse = traverse->link;
  }

  traverse->link = new_pop_node(item, age);

}
 
 //Search hash to get population statistics
pop_ptr pop_hash_search(PHash records, char* item){
  if(records == NULL) return NULL;

  int h = hash(item);

  pop_ptr traverse = records[h].start;

  while(traverse){
    if(!strcmp(traverse->country_name, item)){
      return traverse;
    }

    traverse = traverse->link;
  }

  return NULL;
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