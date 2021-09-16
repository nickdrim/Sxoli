#include "header.h"

LNode vaccineStatusBloom(HTable records, char* citizenID, char* virusName,int flag){
  //If hash table is empty no records
  if(records == NULL){
    printf("NOT VACCINATED\n");
    return NULL;
  }

  int h = hash(virusName);

  //If hashed index is NULL virus does not exist
  if(records[h].start == NULL){
    printf("%s\n",virusName);
    printf("NOT VACCINATED\n");
    return NULL;
  }else{
    HNode traverse = records[h].start;

    while(traverse){
      if(!strcmp(traverse->disease, virusName)){
        //Check Bloom filter to see if citizen is possibly vaccinated
        if(bloom_filter_check(traverse->vaccinated, citizenID)){
          //if vaccine status bloom was called from vaccine status
          //search if citizen is truly vaccinated or not
          if(flag){  
            
            LNode temp = skip_lists_search(traverse->vaccinated_levels, traverse->vaccinated_height, atoi(citizenID));

            if(temp){
              printf("VACCINATED ON %s\n",temp->data->date);
              return temp;
            }else{
              temp = list_node_search(traverse->vaccinated_list, atoi(citizenID));
              if(temp){
                printf("VACCINATED ON %s\n",temp->data->date);
                return temp;
              }else{
                printf("NOT VACCINATED\n");
                return NULL;
              }
            }
          }else{
            printf("MAYBE\n");
            return NULL;
          }  
        }else{
          printf("NOT VACCINATED\n");
          return NULL;
        }

      }

      traverse = traverse->link;
    }

    printf("NOT VACCINATED\n");
    return NULL;
  }
}

void vaccineStatus(HTable records, char* citizenID, char* virusName){
  if(records == NULL){
    printf("NOT VACCINATED\n");
    return;
  }

  if(virusName){
    //if virus name is given call vaccine status bloom
    vaccineStatusBloom(records, citizenID, virusName, 1);
  }else{
    //Else check for all viruses
    for(int i = 0 ; i < get_hashtable_size() ; i++){
      if(records[i].start != NULL){
        
        HNode traverse = records[i].start;

        while(traverse){

          if(bloom_filter_check(traverse->vaccinated, citizenID)){
            LNode temp = skip_lists_search(traverse->vaccinated_levels, traverse->vaccinated_height, atoi(citizenID));
            if(temp) printf("%s %s %s\n",temp->data->virusName, temp->data->vaccine, temp->data->date);
            else{
              temp = list_node_search(traverse->vaccinated_list, atoi(citizenID));
              if(temp) printf("%s %s %s\n",temp->data->virusName, temp->data->vaccine, temp->data->date);
              else printf("%s NO\n",traverse->disease);
            }
          }else{
            printf("%s NO\n",traverse->disease);
          }

          traverse = traverse->link;
        }

      }
    }

  }

}

void insertCitizenRecord(HTable records, CHTable citizen_records, char** commands, int commands_size, int flag){
  //If records is NULL hash table is not initialized.Nowhere to add record
  if(records == NULL || commands == NULL){
    printf("empty\n");
    return;
  }

  char** new_item = NULL;
  //if insertCitizenRecord is called directly remove /insertCitizenRecord from commands
  if(!flag){
    new_item = malloc(sizeof(char*) * 8);

    for(int i = 0 ; i < 8 ; i++){
      if(i == 7 && commands_size == 8){
        new_item[i] = NULL;
      }else{
        new_item[i] = malloc(strlen(commands[i+1]) + 1);
        strcpy(new_item[i], commands[i+1]);
      }
    }
  }else{
    //if called from vaccinate now commands is ready
    new_item = commands;
  }

  //Search citizen hash table to see if name/lastname/age/country match with the citizen id
  if(citizen_hash_search(citizen_records, new_item) == 0){
    printf("error with record . Record with id %s exists with other name/lastname/age/country\n",new_item[citizen]);
    return;
  }else if(citizen_hash_search(citizen_records, new_item) == 2){
    //If record id is new add it to citizen hash table
    citizen_hash_insert(citizen_records, new_item);
  }

  int h = hash(new_item[virus_id]);

  //add record to hash table
  if(records[h].start == NULL){
    records[h].start = new_hash_node(new_item);
  }else if(!strcmp(records[h].start->disease,new_item[virus_id])){
    hash_insert_at_node(records[h].start, new_item);
  }else if(records[h].start->link == NULL){
    records[h].start->link = new_hash_node(new_item);
  }else{

    HNode traverse = records[h].start;

    while(traverse->link){

      if(!strcmp(traverse->disease,new_item[virus_id])){
        hash_insert_at_node(traverse, new_item);
        if(!flag) free(new_item);
        return;
      }

      traverse = traverse->link;

    }

    traverse->link = new_hash_node(new_item);

  }

  //free new item if not called from vaccinatedNow
  for(int i = 0 ; i < 8 ; i++){
    if(new_item != NULL && !flag) free(new_item[i]);
  }

  if(!flag) free(new_item);
}

void listNonVaccinatedPersons(HTable records, char* virusName){
  if(records == NULL) return;

  int h = hash(virusName);

  if(records[h].start == NULL){
    printf("%s virus name does not exist\n", virusName);
    return;
  }

  //Traverse and print not vaccinated list
  HNode traverse = records[h].start;

  while(traverse){
    if(!strcmp(traverse->disease, virusName)){
      list_print(traverse->unvaccinated_list);
      return;
    }

    traverse = traverse->link;
  }

  printf("%s virus name does not exist\n", virusName);

}

void vaccinateNow(HTable records, CHTable citizen_records, char** item){
  if(records == NULL){
    printf("Record does not exist 1\n");
    return;
  }

  //clear /vaccinateNow from commands
  char** new_item = malloc(sizeof(char *) * 8);
  for(int i = 1 ; i < 7 ; i++){
    new_item[i-1] = malloc(strlen(item[i]) + 1);
    strcpy(new_item[i-1], item[i]);
  }

  new_item[yes_no] = malloc(strlen("YES") + 1);
  strcpy(new_item[yes_no], "YES");
  new_item[vac_date] = create_date();

  //Search citizen hash table to see if name/lastname/age/country match with the citizen id
  if(citizen_hash_search(citizen_records, new_item) == 0){
    printf("error with record . Record with id %s exists with other name/lastname/age/country\n",item[citizen]);
    
    for(int i = 0 ; i < 8 ; i++){
      free(new_item[i]);
    }  
    free(new_item);

    return;
  }else if(citizen_hash_search(citizen_records, new_item) == 2){
    //If there is no record already with that id we cant vaccinate
    printf("Record does not exist\n");

    for(int i = 0 ; i < 8 ; i++){
      free(new_item[i]);
    }
    free(new_item);

    return;
  }

  //Call vaccineStatusBloom to see if citizen has already been vaccinated
  LNode temp = vaccineStatusBloom(records, new_item[citizen], new_item[virus_id], 1);
  if(temp){
    printf("ERROR: CITIZEN %d ALREADY VACCINATED ON %s\n",temp->data->citizenID, temp->data->date);
    
    for(int i = 0 ; i < 8 ; i++){
      free(new_item[i]);
    }
    free(new_item);

    return;
  }

  int h = hash(new_item[virus_id]);
  //check if virus exists
  if(records[h].start == NULL){
    printf("Virus does not exist\n");
    
    for(int i = 0 ; i < 8 ; i++){
      free(new_item[i]);
    }
    free(new_item);
    
    return;
  }

  HNode traverse = records[h].start;

  while(traverse){
    if(!strcmp(traverse->disease, new_item[virus_id])){
      if(!bloom_filter_check(traverse->unvaccinated, new_item[citizen])){
        printf("Record does not exist\n");

        for(int i = 0 ; i < 8 ; i++){
          free(new_item[i]);
        }
        free(new_item);

        return;
      }
      
      if(traverse->unvaccinated_levels == NULL){
        //delete record from unvaccinated and add again with insertCitizenRecord
        if(list_delete(traverse->unvaccinated_list, atoi(new_item[citizen]))){
          insertCitizenRecord(records, citizen_records, new_item, 8, 1);
        }else{
          printf("Record does not exits\n");
        }

        for(int i = 0 ; i < 8 ; i++){
          free(new_item[i]);
        }
        free(new_item);

        return;
      }

      SList temp = traverse->unvaccinated_levels[traverse->unvaccinated_height - 1];

      //If record id is at the start of the list rearrange list and skip lists head
      if(temp->skip_id == atoi(new_item[citizen])){
        int next_id = traverse->unvaccinated_list->next->data->citizenID;

        LNode delete_list_node = traverse->unvaccinated_list;

        for(int i = 0 ; i <= traverse->unvaccinated_height - 1 ; i++){
          SList delete = traverse->unvaccinated_levels[i];
          if(delete->right != NULL){
            if(traverse->unvaccinated_levels[i]->right->skip_id == next_id){
              traverse->unvaccinated_levels[i] = delete->right;
              delete->right = NULL;
              skip_list_free(delete);
            }else{
              if(delete->level == 1){
                SList new_node = new_skip_node(delete_list_node->next, NULL);
                new_node->right = delete->right;
                traverse->unvaccinated_levels[i] = new_node;
                delete->right = NULL;
                skip_list_free(delete);
              }else{
                SList new_node = new_skip_node(NULL, traverse->unvaccinated_levels[i-1]);
                new_node->right = delete->right;
                traverse->unvaccinated_levels[i] = new_node;
                delete->right = NULL;
                skip_list_free(delete);
              }
            }
          }else{
            if(delete->level == 1){
              SList new_node = new_skip_node(delete_list_node->next, NULL);
              new_node->right = delete->right;
              traverse->unvaccinated_levels[i] = new_node;
              delete->right = NULL;
              skip_list_free(delete);
            }else{
              SList new_node = new_skip_node(NULL, traverse->unvaccinated_levels[i-1]);
              new_node->right = delete->right;
              traverse->unvaccinated_levels[i] = new_node;
              delete->right = NULL;
              skip_list_free(delete);
            }
          }
        }
        traverse->unvaccinated_list = delete_list_node->next;
        delete_list_node->next = NULL;

        list_free(delete_list_node);

        insertCitizenRecord(records, citizen_records, new_item, 8, 1);

        for(int i = 0 ; i < 8 ; i++){
          free(new_item[i]);
        }
        free(new_item);

        return;
      }else{
        //Else use skip lists to find and delete record
        if(skip_lists_search_delete(traverse->unvaccinated_levels, traverse->unvaccinated_height, atoi(new_item[citizen]))){
          insertCitizenRecord(records, citizen_records, new_item, 8, 1);
        }

        for(int i = 0 ; i < 8 ; i++){
          free(new_item[i]);
        }
        free(new_item);

        return;
      }

    }

    traverse = traverse->link;
  }

}

void populationStatus(HTable records, PHash country_records, char** commands, int commands_size){
  if(records == NULL) return;

  //if country is given
  if(commands_size == 5){

    //Search virus hash table
    HNode disease_node = hash_search(records, commands[2]);

    if(disease_node){
      //Search population hash table to find population stats for given country
      pop_ptr pop = pop_hash_search(country_records, commands[1]);
      //print statistics on given dates
      country_hash_print_statistics(disease_node->country_table, NULL, pop, commands[1], commands[3], commands[4], 0);
    }else{
      printf("Disease %s not found\n", commands[2]);
      return;
    }
  }else{
    //Search virus hash table
    HNode disease_node = hash_search(records, commands[1]);

    if(disease_node){
      //print statistics on given dated for all countries
      country_hash_search_print_statistics(disease_node->country_table, country_records, commands, 0);
    }else{
      printf("Disease %s not found\n", commands[1]);
      return;
    }

  }

}

void popStatusByAge(HTable records, PHash country_records, char** commands, int commands_size){
 if(records == NULL) return;
 
 //Same logic as populationStatus
 if(commands_size == 5){

    HNode disease_node = hash_search(records, commands[2]);

    if(disease_node){
      pop_ptr pop = pop_hash_search(country_records, commands[1]);
      country_hash_print_statistics(disease_node->country_table, NULL, pop, commands[1], commands[3], commands[4], 1);
    }else{
      printf("Disease %s not found\n", commands[2]);
      return;
    }
  }else{

    HNode disease_node = hash_search(records, commands[1]);

    if(disease_node){
      country_hash_search_print_statistics(disease_node->country_table, country_records, commands, 1);
    }else{
      printf("Disease %s not found\n", commands[1]);
      return;
    }

  }
}