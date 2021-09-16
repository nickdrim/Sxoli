#include "header.h"

const float load_factor_2 = 0.6;
int size = 0;
int bloom_size = 0;

//needed to initialize other hash tables.
int get_hashtable_size(void){
  return size;
}

//hash function
int hash(char* key){
  unsigned int h=0;
	int i;

	for(i=0 ; i < strlen(key) ; i++){
		h=(h<<5 | h>>27);
		h+=(unsigned int)key[i];
	}

	return h % size;
}

//Initializes the hash table
HTable hash_initialize(int row_size, int bloom){

  bloom_size = bloom;

  size = row_size / load_factor_2;
  HTable temp = malloc(sizeof(hashtable) * size);

  for(int i = 0 ; i < size ; i++){
    temp[i].start = NULL;
  }

  return temp;
}

//Insert record to hash table
void hash_insert(HTable records, char** item){
  if(records == NULL) return;

  int h = hash(item[virus_id]);

  hash_list_insert(&(records[h].start), item);
}

//Insert record to correct node of hash list
void hash_list_insert(HNode *head, char** item){
  if(*head == NULL){
    //if list is empty
    *head = new_hash_node(item);
    return;
  }else if(!strcmp((*head)->disease, item[virus_id])){
    //else if virus is at the head of the list
    hash_insert_at_node(*head, item);
    return;
  }

  HNode traverse = *head;

  //Search until you find the virus node
  while(traverse->link != NULL){
    if(!strcmp(traverse->disease, item[virus_id])){
      hash_insert_at_node(traverse, item);
      return;
    }

    traverse = traverse->link;
  }

  //if virus node not found create it
  traverse->link = new_hash_node(item);

}

//Insert record at a specific node
void hash_insert_at_node(HNode head, char** item){
  
  //if answer is yes insert it at vaccinated list.
  if(!strcmp(item[yes_no], "YES")){
    //check if record possibly exists in list
    if(bloom_filter_check(head->vaccinated, item[citizen])){
      //if it possibly exists check to see if it actually already exists
      if(skip_lists_search(head->vaccinated_levels, head->vaccinated_height, atoi(item[citizen]))){
        printf("Records already exists %s\n",item[citizen]);
        return;
      }else if(list_search(head->vaccinated_list, atoi(item[citizen]))){
        printf("Records already exists %s\n",item[citizen]);
        return;
      }else{
        //else if does not exists in vaccinated list check if it possibly exists in unvaccinated list
        if(bloom_filter_check(head->unvaccinated, item[citizen])){
          //if it possibly exists check to see if it actually already exists
          if(skip_lists_search(head->unvaccinated_levels, head->unvaccinated_height, atoi(item[citizen]))){
            printf("Record %s already exists in unvaccinated list\n",item[citizen]);
            return;
          }else if(list_search(head->unvaccinated_list, atoi(item[citizen]))){
            printf("Record %s already exists in unvaccinated list\n",item[citizen]);
            return;
          }
        }

        //else FALSE POSITIVE. Insert at node

        //if skip list is not initialized and records are over 100
        if(head->vaccinated_levels == NULL && head->no_vaccinated == 100){
          
          //Initialize skip list
          head->vaccinated_height++;
          head->vaccinated_levels = malloc(sizeof(SList) * head->vaccinated_height);
          head->vaccinated_levels[head->vaccinated_height - 1] = new_skip_list(head->vaccinated_list);

          //if record id is smaller than head of skip list place it at the begining of the list
          if(head->vaccinated_levels[head->vaccinated_height - 1]->skip_id > atoi(item[citizen])){
            //Swap list head
            LNode temp = new_list_node(item);
            temp->next = head->vaccinated_list;
            head->vaccinated_list = temp;

            //Swap skip list head
            SList insert = new_skip_node(temp, NULL);
            insert->right = head->vaccinated_levels[0];
            head->vaccinated_levels[0] = insert;

          }else{
            //search list to insert at the correct location
            skip_lists_search_insert(head->vaccinated_levels, head->vaccinated_list, head->vaccinated_height, item);          
          }

          country_hash_insert(head->country_table, item);
        }else if(head->vaccinated_levels != NULL && head->no_vaccinated % 100 == 0){
          //if skip lists are initialized and 100 more records have been added increase height
          head->vaccinated_height++;
          head->vaccinated_levels = realloc(head->vaccinated_levels, sizeof(SList) * head->vaccinated_height);

          head->vaccinated_levels[head->vaccinated_height - 1] = skip_increase_height(head->vaccinated_levels[head->vaccinated_height - 2]);

          //if record id is smaller than head of skip list place it at the begining of the list
          if(head->vaccinated_levels[head->vaccinated_height - 1]->skip_id > atoi(item[citizen])){
            LNode temp = new_list_node(item);
            temp->next = head->vaccinated_list;
            head->vaccinated_list = temp;

            //for all levels swap head of skip list
            for(int i = 0 ; i < head->vaccinated_height - 1 ; i++){
              if(head->vaccinated_levels[i]->level == 1){
                SList insert = new_skip_node(temp, NULL);
                insert->right = head->vaccinated_levels[i];
                head->vaccinated_levels[i] = insert;
              }else{
                SList insert = new_skip_node(NULL, head->vaccinated_levels[i-1]);
                insert->right = head->vaccinated_levels[i];
                head->vaccinated_levels[i] = insert;
              }
            }
          }else{
            skip_lists_search_insert(head->vaccinated_levels, head->vaccinated_list, head->vaccinated_height, item);          
          }
              
          country_hash_insert(head->country_table, item);
        }else{
          //if skip list is initialized insert using skip list
          if(head->vaccinated_levels){

            //same logic as above(swap head if smaller id)
            if(head->vaccinated_levels[head->vaccinated_height - 1]->skip_id > atoi(item[citizen])){
              LNode temp = new_list_node(item);
              temp->next = head->vaccinated_list;
              head->vaccinated_list = temp;

              for(int i = 0 ; i < head->vaccinated_height - 1 ; i++){
                if(head->vaccinated_levels[i]->level == 1){
                  SList insert = new_skip_node(temp, NULL);
                  insert->right = head->vaccinated_levels[i];
                  head->vaccinated_levels[i] = insert;
                }else{
                  SList insert = new_skip_node(NULL, head->vaccinated_levels[i-1]);
                  insert->right = head->vaccinated_levels[i];
                  head->vaccinated_levels[i] = insert;
                }
              }
            }else{
              skip_lists_search_insert(head->vaccinated_levels, head->vaccinated_list, head->vaccinated_height, item);          
            }

          }else{
            //if skip list is not initialized insert it using list
            list_insert(&(head->vaccinated_list), item);
          }
          country_hash_insert(head->country_table, item);
        }

        //Increase vaccinated persons and update the bloom filter so that we know its part of the set
        head->no_vaccinated++;
        bloom_filter_update(head->vaccinated, item[citizen]);
      }
    }else{
      //if its not already in the vaccinated set check if it exists in unvaccinated set
      if(bloom_filter_check(head->unvaccinated, item[citizen])){
        //if it exists search unvaccinated lists
        if(skip_lists_search(head->unvaccinated_levels, head->unvaccinated_height, atoi(item[citizen]))){
          printf("Records already exists %s\n",item[citizen]);
          return;
        }else if(list_search(head->unvaccinated_list, atoi(item[citizen]))){
          printf("Records already exists %s\n",item[citizen]);
          return;
        }
      }

      //Same process as above
      if(head->vaccinated_levels == NULL && head->no_vaccinated == 100){
          
        head->vaccinated_height++;
        head->vaccinated_levels = malloc(sizeof(SList) * head->vaccinated_height);
        head->vaccinated_levels[head->vaccinated_height - 1] = new_skip_list(head->vaccinated_list);

        if(head->vaccinated_levels[head->vaccinated_height - 1]->skip_id > atoi(item[citizen])){
          LNode temp = new_list_node(item);
          temp->next = head->vaccinated_list;
          head->vaccinated_list = temp;
          
          for(int i = 0 ; i < head->vaccinated_height - 1 ; i++){
            if(head->vaccinated_levels[i]->level == 1){
              SList insert = new_skip_node(temp, NULL);
              insert->right = head->vaccinated_levels[i];
              head->vaccinated_levels[i] = insert;
            }else{
              SList insert = new_skip_node(NULL, head->vaccinated_levels[i-1]);
              insert->right = head->vaccinated_levels[i];
              head->vaccinated_levels[i] = insert;
            }
          }
        }else{
          skip_lists_search_insert(head->vaccinated_levels, head->vaccinated_list, head->vaccinated_height, item);          
        }

        country_hash_insert(head->country_table, item);
      }else if(head->vaccinated_levels && head->no_vaccinated % 100 == 0){
        head->vaccinated_height++;
        head->vaccinated_levels = realloc(head->vaccinated_levels, sizeof(SList) * head->vaccinated_height);

        head->vaccinated_levels[head->vaccinated_height - 1] = skip_increase_height(head->vaccinated_levels[head->vaccinated_height - 2]);
        
        if(head->vaccinated_levels[head->vaccinated_height - 1]->skip_id > atoi(item[citizen])){
          LNode temp = new_list_node(item);
          temp->next = head->vaccinated_list;
          head->vaccinated_list = temp;
          
          for(int i = 0 ; i < head->vaccinated_height - 1 ; i++){
            if(head->vaccinated_levels[i]->level == 1){
              SList insert = new_skip_node(temp, NULL);
              insert->right = head->vaccinated_levels[i];
              head->vaccinated_levels[i] = insert;
            }else{
              SList insert = new_skip_node(NULL, head->vaccinated_levels[i-1]);
              insert->right = head->vaccinated_levels[i];
              head->vaccinated_levels[i] = insert;
            }
          }
        }else{
          skip_lists_search_insert(head->vaccinated_levels, head->vaccinated_list, head->vaccinated_height, item);          
        }
        
        country_hash_insert(head->country_table, item);
      }else{
        if(head->vaccinated_levels){
          
          if(head->vaccinated_levels[head->vaccinated_height - 1]->skip_id > atoi(item[citizen])){
            LNode temp = new_list_node(item);
            temp->next = head->vaccinated_list;
            head->vaccinated_list = temp;
            
            for(int i = 0 ; i < head->vaccinated_height - 1 ; i++){
              if(head->vaccinated_levels[i]->level == 1){
                SList insert = new_skip_node(temp, NULL);
                insert->right = head->vaccinated_levels[i];
                head->vaccinated_levels[i] = insert;
              }else{
                SList insert = new_skip_node(NULL, head->vaccinated_levels[i-1]);
                insert->right = head->vaccinated_levels[i];
                head->vaccinated_levels[i] = insert;
              }
            }
          }else{
            skip_lists_search_insert(head->vaccinated_levels, head->vaccinated_list, head->vaccinated_height, item);          
          }
          
        }else{
          list_insert(&(head->vaccinated_list), item);
        }
        country_hash_insert(head->country_table, item);
      }
      head->no_vaccinated++;
      bloom_filter_update(head->vaccinated, item[citizen]);
    }
  }else{
    //Same process as if answer was YES but this time for NO, and names are adjusted

    if(bloom_filter_check(head->unvaccinated, item[citizen])){
      if(skip_lists_search(head->unvaccinated_levels, head->unvaccinated_height, atoi(item[citizen]))){
        printf("Records already exists %s\n",item[citizen]);
        return;
      }else if(list_search(head->unvaccinated_list, atoi(item[citizen]))){
        printf("Records already exists %s\n",item[citizen]);
        return;
      }else{
        if(bloom_filter_check(head->vaccinated, item[citizen])){
          if(skip_lists_search(head->vaccinated_levels, head->vaccinated_height, atoi(item[citizen]))){
            printf("Records already exists %s\n",item[citizen]);
            return;
          }if(list_search(head->vaccinated_list, atoi(item[citizen]))){
            printf("Record already exists in vaccinated list\n");
            return;
          }
        }

        if(head->unvaccinated_levels == NULL && head->no_unvaccinated == 100){
          
          head->unvaccinated_height++;
          head->unvaccinated_levels = malloc(sizeof(SList) * head->unvaccinated_height);
          head->unvaccinated_levels[head->unvaccinated_height - 1] = new_skip_list(head->unvaccinated_list);
          
          if(head->unvaccinated_levels[head->unvaccinated_height - 1]->skip_id > atoi(item[citizen])){
            LNode temp = new_list_node(item);
            temp->next = head->unvaccinated_list;
            head->unvaccinated_list = temp;
            
            for(int i = 0 ; i < head->unvaccinated_height - 1 ; i++){
              if(head->unvaccinated_levels[i]->level == 1){
                SList insert = new_skip_node(temp, NULL);
                insert->right = head->unvaccinated_levels[i];
                head->unvaccinated_levels[i] = insert;
              }else{
                SList insert = new_skip_node(NULL, head->unvaccinated_levels[i-1]);
                insert->right = head->unvaccinated_levels[i];
                head->unvaccinated_levels[i] = insert;
              }
            }
          }else{
            skip_lists_search_insert(head->unvaccinated_levels, head->unvaccinated_list, head->unvaccinated_height, item);          
          }

          country_hash_insert(head->country_table, item);
        }else if(head->unvaccinated_levels && head->no_unvaccinated % 100 == 0){
          head->unvaccinated_height++;
          head->unvaccinated_levels = realloc(head->unvaccinated_levels, sizeof(SList) * head->unvaccinated_height);

          head->unvaccinated_levels[head->unvaccinated_height - 1] = skip_increase_height(head->unvaccinated_levels[head->unvaccinated_height - 2]);
          
          if(head->unvaccinated_levels[head->unvaccinated_height - 1]->skip_id > atoi(item[citizen])){
            LNode temp = new_list_node(item);
            temp->next = head->unvaccinated_list;
            head->unvaccinated_list = temp;
            
            for(int i = 0 ; i < head->unvaccinated_height - 1 ; i++){
              if(head->unvaccinated_levels[i]->level == 1){
                SList insert = new_skip_node(temp, NULL);
                insert->right = head->unvaccinated_levels[i];
                head->unvaccinated_levels[i] = insert;
              }else{
                SList insert = new_skip_node(NULL, head->unvaccinated_levels[i-1]);
                insert->right = head->unvaccinated_levels[i];
                head->unvaccinated_levels[i] = insert;
              }
            }
          }else{
            skip_lists_search_insert(head->unvaccinated_levels, head->unvaccinated_list, head->unvaccinated_height, item);          
          }
          
          country_hash_insert(head->country_table, item);
        }else{
          if(head->unvaccinated_levels){

            if(head->unvaccinated_levels[head->unvaccinated_height - 1]->skip_id > atoi(item[citizen])){
              LNode temp = new_list_node(item);
              temp->next = head->unvaccinated_list;
              head->unvaccinated_list = temp;
              
              for(int i = 0 ; i < head->unvaccinated_height - 1 ; i++){
                if(head->unvaccinated_levels[i]->level == 1){
                  SList insert = new_skip_node(temp, NULL);
                  insert->right = head->unvaccinated_levels[i];
                  head->unvaccinated_levels[i] = insert;
                }else{
                  SList insert = new_skip_node(NULL, head->unvaccinated_levels[i-1]);
                  insert->right = head->unvaccinated_levels[i];
                  head->unvaccinated_levels[i] = insert;
                }
              }
            }else{
              skip_lists_search_insert(head->unvaccinated_levels, head->unvaccinated_list, head->unvaccinated_height, item);          
            }
          }else{
            list_insert(&(head->unvaccinated_list), item);
          }
          country_hash_insert(head->country_table, item);
        }
        head->no_unvaccinated++;
        bloom_filter_update(head->unvaccinated, item[citizen]);
      }
    }else{
      if(bloom_filter_check(head->vaccinated, item[citizen])){
        if(skip_lists_search(head->vaccinated_levels, head->vaccinated_height, atoi(item[citizen]))){
          printf("Records already exists %s\n",item[citizen]);
          return;
        }else if(list_search(head->vaccinated_list, atoi(item[citizen]))){
          printf("Records already exists %s\n",item[citizen]);
          return;
        }
      }

      if(head->unvaccinated_levels == NULL && head->no_unvaccinated == 100){
          
        head->unvaccinated_height++;
        head->unvaccinated_levels = malloc(sizeof(SList) * head->unvaccinated_height);
        head->unvaccinated_levels[head->unvaccinated_height - 1] = new_skip_list(head->unvaccinated_list);
        
        if(head->unvaccinated_levels[head->unvaccinated_height - 1]->skip_id > atoi(item[citizen])){
          LNode temp = new_list_node(item);
          temp->next = head->unvaccinated_list;
          head->unvaccinated_list = temp;
          
          for(int i = 0 ; i < head->unvaccinated_height - 1 ; i++){
            if(head->unvaccinated_levels[i]->level == 1){
              SList insert = new_skip_node(temp, NULL);
              insert->right = head->unvaccinated_levels[i];
              head->unvaccinated_levels[i] = insert;
            }else{
              SList insert = new_skip_node(NULL, head->unvaccinated_levels[i-1]);
              insert->right = head->unvaccinated_levels[i];
              head->unvaccinated_levels[i] = insert;
            }
          }
        }else{
          skip_lists_search_insert(head->unvaccinated_levels, head->unvaccinated_list, head->unvaccinated_height, item);          
        }

        country_hash_insert(head->country_table, item);
      }else if(head->unvaccinated_levels && head->no_unvaccinated % 100 == 0){
        head->unvaccinated_height++;
        head->unvaccinated_levels = realloc(head->unvaccinated_levels, sizeof(SList) * head->unvaccinated_height);

        head->unvaccinated_levels[head->unvaccinated_height - 1] = skip_increase_height(head->unvaccinated_levels[head->unvaccinated_height - 2]);
        
        if(head->unvaccinated_levels[head->unvaccinated_height - 1]->skip_id > atoi(item[citizen])){
          LNode temp = new_list_node(item);
          temp->next = head->unvaccinated_list;
          head->unvaccinated_list = temp;
          
          for(int i = 0 ; i < head->unvaccinated_height - 1 ; i++){
            if(head->unvaccinated_levels[i]->level == 1){
              SList insert = new_skip_node(temp, NULL);
              insert->right = head->unvaccinated_levels[i];
              head->unvaccinated_levels[i] = insert;
            }else{
              SList insert = new_skip_node(NULL, head->unvaccinated_levels[i-1]);
              insert->right = head->unvaccinated_levels[i];
              head->unvaccinated_levels[i] = insert;
            }
          }
        }else{
          skip_lists_search_insert(head->unvaccinated_levels, head->unvaccinated_list, head->unvaccinated_height, item);          
        }
        
        country_hash_insert(head->country_table, item);
      }else{
        if(head->unvaccinated_levels){
          if(head->unvaccinated_levels[head->unvaccinated_height - 1]->skip_id > atoi(item[citizen])){
            LNode temp = new_list_node(item);
            temp->next = head->unvaccinated_list;
            head->unvaccinated_list = temp;
            
            for(int i = 0 ; i < head->unvaccinated_height - 1 ; i++){
              if(head->unvaccinated_levels[i]->level == 1){
                SList insert = new_skip_node(temp, NULL);
                insert->right = head->unvaccinated_levels[i];
                head->unvaccinated_levels[i] = insert;
              }else{
                SList insert = new_skip_node(NULL, head->unvaccinated_levels[i-1]);
                insert->right = head->unvaccinated_levels[i];
                head->unvaccinated_levels[i] = insert;
              }
            }
          }else{
            skip_lists_search_insert(head->unvaccinated_levels, head->unvaccinated_list, head->unvaccinated_height, item);          
          }
          
        }else{
          list_insert(&(head->unvaccinated_list), item);
        }
        country_hash_insert(head->country_table, item);
      }

      head->no_unvaccinated++;
      bloom_filter_update(head->unvaccinated, item[citizen]);
    }
  }
}

//Create and retun a new hash node
HNode new_hash_node(char** item){
  
  HNode temp = malloc(sizeof(hashlist));

  temp->disease = malloc(strlen(item[virus_id]) + 1);
  strcpy(temp->disease, item[virus_id]);

  //Initialize filters
  temp->vaccinated = new_bloom_filter(bloom_size);
  temp->unvaccinated = new_bloom_filter(bloom_size);

  temp->link = NULL;

  //Initialize values and filter according to answer
  if(!strcmp(item[yes_no],"YES")){
    bloom_filter_update(temp->vaccinated, item[citizen]);
    temp->no_vaccinated = 1;
    temp->no_unvaccinated = 0;
    temp->unvaccinated_list = NULL;
    temp->vaccinated_list = new_list_node(item);
    
  }else{
    bloom_filter_update(temp->unvaccinated, item[citizen]);
    temp->no_unvaccinated = 1;
    temp->no_vaccinated = 0;
    temp->vaccinated_list = NULL;
    temp->unvaccinated_list = new_list_node(item);
  }

  //Initialize country hash table
  temp->country_table = country_hash_initialize();
  
  country_hash_insert(temp->country_table, item);

  //Initialize skip lists
  temp->vaccinated_levels = NULL;
  temp->unvaccinated_levels = NULL;
  temp->vaccinated_height = 0;
  temp->unvaccinated_height = 0;

  return temp;
}

//Print hash table(No use currently)
void hash_print(HTable records){
  if(records == NULL) return;

  for(int i = 0 ; i < size ; i++){
    if(records[i].start != NULL){
      hash_list_print(records[i].start);
      country_hash_print(records[i].start->country_table);
    }
  }
}

//Seach if item belongs to the hash table and return the found node or NULL
HNode hash_search(HTable records, char* item){
  int h = hash(item);

  HNode traverse = records[h].start;

  while(traverse){
    if(!strcmp(traverse->disease,item)){
      //printf("found %s %d %p\n",traverse->disease, h, traverse);
      return traverse;
    }

    traverse = traverse->link;
  }

  //printf("not found %d\n",h);
  return NULL;
}

//Prints hash list(no use currently)
void hash_list_print(HNode head){
  if(head == NULL) return;

  HNode traverse = head;

  while(traverse){
    printf("Virus name = %s %p\n",traverse->disease,traverse);
    printf("\n\nunvaccinated\n\n");
    skip_lists_print(traverse->unvaccinated_levels, traverse->vaccinated_height);
    list_print(traverse->unvaccinated_list);

    printf("\n\nvaccinated\n\n");
    skip_lists_print(traverse->vaccinated_levels, traverse->unvaccinated_height);
    list_print(traverse->vaccinated_list);

    traverse = traverse->link;
  }
}

//Deallocate memory used by the hash table
void hash_free(HTable records){
  if(records == NULL) return;

  for(int i = 0 ; i < size ; i++){
    if(records[i].start != NULL) hash_list_free(records[i].start);
  }
  
  free(records);
}

//Deallocate memory used by the hash list
void hash_list_free(HNode head){
  if(head == NULL) return;

  hash_list_free(head->link);

  free(head->disease);
  bloom_filter_free(head->vaccinated);
  bloom_filter_free(head->unvaccinated);

  skip_lists_free(head->vaccinated_levels, head->vaccinated_height);
  skip_lists_free(head->unvaccinated_levels, head->unvaccinated_height);

  list_free(head->vaccinated_list);
  list_free(head->unvaccinated_list);

  country_hash_free(head->country_table);

  free(head);

}