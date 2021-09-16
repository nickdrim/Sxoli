#include "header.h"

int hsize = 0;
int total = 0;
int ok = 0;
int denied = 0;

//Initialize country hash table
COUNTable country_hash_initialize(void){

  hsize = 1000;
  COUNTable temp = malloc(sizeof(country_hash) * hsize);

  for(int i = 0 ; i < hsize ; i++){
    temp[i].start = NULL;
  }

  return temp;
}

//New country hash node
country_node new_country_node(char* item){

  country_node temp = malloc(sizeof(country_hashlist));
  temp->country_name = malloc(strlen(item) + 1);
  strcpy(temp->country_name,item);
  temp->virus_list = NULL;
  temp->link = NULL;

  return temp;

}

//Insert item to country hash table
void country_hash_insert(COUNTable records, char* item){
  if(records == NULL) return;

  int h = hash(item, hsize);

  country_hash_list_insert(&(records[h].start), item);
}

//Insert item to hash list (same as hash.c)
void country_hash_list_insert(country_node *head, char* item){
  if(*head == NULL){
    *head = new_country_node(item);
    return;
  }

  country_node traverse = *head;

  while(traverse->link != NULL){
    traverse = traverse->link;
  }

  traverse->link = new_country_node(item);

}

//Free country hash table
void country_hash_free(COUNTable records){
  if(records == NULL) return;

  for(int i = 0 ; i < hsize ; i++){
    if(records[i].start != NULL) country_hash_list_free(records[i].start);
  }
  
  free(records);
}

//Free country hash list
void country_hash_list_free(country_node head){
  if(head == NULL) return;

  country_hash_list_free(head->link);

  free(head->country_name);
  virus_list_free(head->virus_list);

  free(head);
}

/*void country_hash_search_insert(COUNTable records, char* find_country, char* desease, BFilter new_bloom){
  if(records == NULL) return;

  int h = hash(find_country, hsize);

  country_hash_list_search_insert(records[h].start, find_country, desease,new_bloom);

}

void country_hash_list_search_insert(country_node head, char* find_country, char* desease, BFilter new_bloom){
  if(head == NULL) return;

  country_node traverse = head;

  while(traverse){
    if(!strcmp(traverse->country_name,find_country)){
      if(traverse->virus_list == NULL){
        traverse->virus_list = create_new_virus_node(desease, new_bloom);
      }else{
        virus_list_insert(traverse->virus_list,desease,new_bloom);
      }
    }

    traverse = traverse->link;
  }
}*/

//Initializes the bloom filter of virus
void country_hash_initialize_bloom(COUNTable records, char* desease, int* temp_bloom){
  if(records == NULL) return;

  for(int i = 0 ; i < hsize ; i++){
    if(records[i].start != NULL){
      country_node traverse = records[i].start;

      while(traverse){
        if(traverse->virus_list == NULL){
          traverse->virus_list = create_new_virus_node(desease, temp_bloom);
        }else{
          virus_list_insert(traverse->virus_list, desease, temp_bloom);
        }

        traverse = traverse->link;
      }
    }
  }
}

//Updates the containts of a bloom filter(/addVaccinationRecords)
void country_hash_update_bloom(COUNTable records, char* desease, int* new_bloom){
  if(records == NULL) return;

  for(int i = 0 ; i < hsize ; i++){
    country_node traverse = records[i].start;

    while(traverse){
      virus_bloom_update(traverse->virus_list, desease, new_bloom);

      traverse = traverse->link;
    }
  }
}

//Checks if citizen with citizen_id is possibly vaccinated from virus(1 if true 0 if false)
int country_hash_bloom_check(COUNTable records, char* virus, char* citizen_id, char* country, char* date){
  if(records == NULL){
    return 0;
  }
  int h = hash(country, hsize);

  country_node traverse = records[h].start;

  while(traverse){
    if(!strcmp(traverse->country_name,country)){
      return virus_list_check_bloom(traverse->virus_list, virus, citizen_id, date);
    }

    traverse = traverse->link;
  }

  return 0;
}

//Creates a new request to visit another country according to answer(/travelRequest)
void country_new_request(COUNTable records, char* country, char* desease, char* new_date, int answer){
  if(records == NULL) return;

  int h = hash(country, hsize);

  country_node traverse = records[h].start;

  while(traverse){
    if(!strcmp(traverse->country_name,country)){
      return virus_new_request(traverse->virus_list, desease, new_date, answer);
    }

    traverse = traverse->link;
  }
}

//Prints the total requests between two dates for a virus, either for one country(/travelStats)
void country_print_requests(COUNTable records, char* desease, char* start_date, char* end_date, char* country){
  if(records == NULL) return;

  total = 0;
  denied = 0;
  ok = 0;

  //if specific country is given
  if(country != NULL){

    int h = hash(country, hsize);

    country_node traverse = records[h].start;
    while(traverse){
      if(!strcmp(traverse->country_name, country)){
        virus_print_requests(traverse->virus_list, desease, start_date, end_date);
      }

      traverse = traverse->link;
    }

  }


  printf("TOTAL REQUESTS %d\n", total);
  printf("ACCEPTED %d\n",ok);
  printf("REJECTED %d\n", denied);

}

//Prints the total requests between two dates for a virus, either for all countries(/travelStats)
void countries_print_requests(COUNTable* records, int record_size, char* desease, char* start_date, char* end_date){
  if(records == NULL) return;

  total = 0;
  denied = 0;
  ok = 0;

  for(int i = 0 ; i < record_size ; i++){

    COUNTable temp = records[i];

    for(int j = 0 ; j < hsize ; j++){
      country_node traverse = temp[j].start;

      while(traverse){
        virus_print_requests(traverse->virus_list, desease, start_date, end_date);

        traverse = traverse->link;
      }

    }
  }

  printf("TOTAL REQUESTS %d\n", total);
  printf("ACCEPTED %d\n",ok);
  printf("REJECTED %d\n", denied);
}

//virus

//Creates and returns a new virus node(Used while reading fifos)
VNode create_new_virus_node(char* desease, int* new_bloom){

  VNode temp = malloc(sizeof(virus));

  temp->virus_name = malloc(strlen(desease) + 1);
  strcpy(temp->virus_name,desease);
  temp->bloom = bloom_filter_copy(new_bloom, bloom_size_get());
  temp->req = NULL;
  temp->right = NULL;
  
  return temp;

}

//Inserts a new virus to list or updates the bloom filter of an already existing virus
void virus_list_insert(VNode head, char* desease, int* new_bloom){
  if(head == NULL) return;

  VNode traverse = head;

  while(traverse->right){
    if(!strcmp(traverse->virus_name, desease)){
      bloom_filter_renew(traverse->bloom, new_bloom);
      return;
    }
    traverse = traverse->right;
  }

  traverse->right = create_new_virus_node(desease, new_bloom);
}

//Creates a new request according to answer
void virus_new_request(VNode head, char* desease, char* new_date, int answer){
  if(head == NULL) return;

  VNode traverse = head;

  while(traverse){
    if(!strcmp(traverse->virus_name, desease)){
      if(traverse->req == NULL){
        traverse->req = create_new_apply_node(new_date, answer);
      }else{
        apply_list_insert(&(traverse->req), new_date, answer);
      }

      return;
    }

    traverse = traverse->right;
  }
  
}

//Locates requested virus and checks the travel requests made between two dates
void virus_print_requests(VNode head, char* desease, char* start_date, char* end_date){
  if(head == NULL){
    printf("No records for %s virus\n",desease);
    return;
  }

  VNode traverse = head;

  while(traverse){
    if(!strcmp(traverse->virus_name, desease)){
      apply_list_print_requests(traverse->req, start_date, end_date);

      return;
    }

    traverse = traverse->right;
  }
}

//Updates the bloom filter of a virus
void virus_bloom_update(VNode head, char* desease, int* new_bloom){
  if(head == NULL) return;

  VNode traverse = head;

  while(traverse){
    if(!strcmp(traverse->virus_name, desease)){
      bloom_filter_renew(traverse->bloom, new_bloom);
      return;
    }

    traverse = traverse->right;
  }
}

//Checks if citizen with citizen_id is possibly vaccinated against desease
//If he is not add the request immediately(/travelRequest)
int virus_list_check_bloom(VNode head, char* desease, char* citizen_id , char* date){
  if(head == NULL) return 0;

  VNode traverse = head;

  while(traverse){
    if(!strcmp(traverse->virus_name, desease)){
      if(bloom_filter_check(traverse->bloom, citizen_id)){
        return 1;
      }else{
        if(traverse->req == NULL){
          traverse->req = create_new_apply_node(date,0);
        }else{
          apply_list_insert(&(traverse->req), date, 0);
        }

        return 0;
      }
    }

    traverse = traverse->right;
  }

  return 0;
}

//Frees a virus node
void virus_list_free(VNode head){
  if(head == NULL) return;

  virus_list_free(head->right);
  apply_list_free(head->req);
  free(head->virus_name);
  bloom_filter_free(head->bloom);

  free(head);

}



//apply

//Creates a new request
ANode create_new_apply_node(char* new_date, int response){
  
  ANode temp = malloc(sizeof(apply));

  temp->date = malloc(strlen(new_date) + 1);
  strcpy(temp->date,new_date);
  temp->answer = response;
  temp->next = NULL;

  printf("NEW REQUEST RECORDED\n");
  return temp;

}

//Adds a new request to request list according to date in ascending order
void apply_list_insert(ANode* head, char* new_date, int response){
  if(*head == NULL) return;

  ANode temp = create_new_apply_node(new_date, response);

  if(compareDate((*head)->date, new_date) > 0){  

    temp->next = (*head);
    *head = temp;

    return;
  }else if((*head)->next == NULL){
    (*head)->next = temp;
    return;
  }

  ANode traverse = (*head)->next;
  ANode previous = *head;

  while(traverse){
    if(compareDate(traverse->date, new_date) > 0){
      temp->next = traverse;
      previous->next = temp;
      return;
    }

    previous = traverse;
    traverse = traverse->next;
  }

  previous->next = temp;
}

//Counts all request between two dates and the number of rejected requests and ok requests
//Values will be printed in country_hash_print_request or countries_hash_print_request
void apply_list_print_requests(ANode head, char* start_date, char* end_date){
  if(head == NULL){
    return;
  }

  ANode traverse = head;

  while(traverse){
    //traverse date bigger or equal to start date end traverse_date smaller or equal to end date
    if((compareDate(start_date, traverse->date) <= 0) && (compareDate(end_date, traverse->date) >= 0)){
      total++;

      if(traverse->answer){
        ok++;
      }else{
        denied++;
      }
    }

    traverse = traverse->next;

  }

}

//Frees a request node
void apply_list_free(ANode head){
  if(head == NULL) return;

  apply_list_free(head->next);
  free(head->date);

  free(head);
}