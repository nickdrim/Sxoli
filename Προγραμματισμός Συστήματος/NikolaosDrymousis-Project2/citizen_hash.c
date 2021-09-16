#include "header.h"

int hash_size = 0;
const float load_factor = 0.6;

//Initialize citizen hash table
CHTable citizen_hash_initialize(void){

  hash_size = 1000;
  CHTable temp = malloc(sizeof(citizen_hash) * hash_size);

  for(int i = 0 ; i < hash_size ; i++){
    temp[i].start = NULL;
  }

  return temp;
}

//Create citizen information node
check_info new_citizen_info(char** item){

  check_info temp = malloc(sizeof(citizen_info));

  temp->age = atoi(item[years]);

  temp->country = malloc(strlen(item[location]) + 1);
  strcpy(temp->country, item[location]);

  temp->firstName = malloc(strlen(item[fname]) + 1);
  strcpy(temp->firstName, item[fname]);

  temp->lastName = malloc(strlen(item[lname]) + 1);
  strcpy(temp->lastName, item[lname]);

  return temp;
}

//Create new citizen hash node
CHNode new_citizen_node(char** item){

  CHNode temp = malloc(sizeof(citizen_hashlist));
  temp->next = NULL;
  temp->cit_data = new_citizen_info(item);
  
  temp->cit_id = malloc(strlen(item[citizen]) + 1);
  strcpy(temp->cit_id, item[citizen]);

  return temp;

}

//Insert new citizen record to hash table
void citizen_hash_insert(CHTable records, char** item){
  if(records == NULL) return;

  int h = hash(item[citizen], hash_size);

  if(records[h].start == NULL){
    records[h].start = new_citizen_node(item);
    return;
  }else if(!strcmp(records[h].start->cit_id, item[citizen])){
    return;
  }else{

    CHNode traverse = records[h].start;

    while(traverse->next){

      if(!strcmp(traverse->cit_id, item[citizen]))
        return;

      traverse = traverse->next;
    }

    traverse->next = new_citizen_node(item);
  }
}

int citizen_hash_search_id(CHTable records, char* item){
  if(records == NULL) return 0;

  int h = hash(item, hash_size);

  CHNode traverse = records[h].start;

  while(traverse){
    
    if(!strcmp(traverse->cit_id,item)){
      return 1;
    }

    traverse = traverse->next;
  }

  return 0;
}

check_info citizen_hash_return_info(CHTable records, char* item){
  if(records == NULL) return NULL;

  int h = hash(item, hash_size);

  CHNode traverse = records[h].start;

  while(traverse){
    
    if(!strcmp(traverse->cit_id,item)){
      return traverse->cit_data;
    }

    traverse = traverse->next;
  }

  return NULL;
}

//Search citizen hash table to find item
int citizen_hash_search(CHTable records, char** item){
  //if 2 than it is not found
  if(records == NULL) return 2;

  int h = hash(item[citizen], hash_size);

  CHNode traverse = records[h].start;

  while(traverse){
    
    if(!strcmp(traverse->cit_id,item[citizen])){
      return compare_record(traverse->cit_data, item);
    }

    traverse = traverse->next;
  }

  return 2;
}

//If 1 is returned item is new
//If 0 is returned information of item does not match record
int compare_record(check_info cit_data, char** item){
  if(cit_data == NULL) return 1;

  if(strcmp(cit_data->country, item[location])) return 0;

  if(strcmp(cit_data->firstName, item[fname])) return 0;

  if(strcmp(cit_data->lastName, item[lname])) return 0;

  if(cit_data->age != atoi(item[years])) return 0;

  return 1;
}

//Free citizen hash table
void citizen_hash_free(CHTable citizen_records){
  if(citizen_records == NULL) return;

  for(int i = 0 ; i < hash_size ; i++){
    if(citizen_records[i].start != NULL) citizen_hashlist_free(citizen_records[i].start);
  }

  free(citizen_records);
}

//free citizen list
void citizen_hashlist_free(CHNode head){
  if(head == NULL) return;

  citizen_hashlist_free(head->next);
  citizen_info_free(head->cit_data);
  free(head->cit_id);
  free(head);
}

//free citizen info
void citizen_info_free(check_info data){
  if(data == NULL) return;

  free(data->country);
  free(data->firstName);
  free(data->lastName);

  free(data);
}