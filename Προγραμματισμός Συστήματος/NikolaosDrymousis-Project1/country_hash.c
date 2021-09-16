#include "header.h"

int hsize = 0;

//Initialize country hash table
COUNTable country_hash_initialize(void){

  hsize = get_hashtable_size();
  COUNTable temp = malloc(sizeof(country_hash) * hsize);

  for(int i = 0 ; i < hsize ; i++){
    temp[i].start = NULL;
  }

  return temp;
}

//New country hash node
country_node new_country_node(char** item){

  country_node temp = malloc(sizeof(country_hashlist));
  
  temp->country_name = malloc(strlen(item[location]) + 1);
  strcpy(temp->country_name, item[location]);

  temp->population = 1;
  temp->skip_height = 0;
  temp->country_skip = NULL;

  if(!strcmp(item[yes_no],"YES")){
    temp->pop_vaccinated = 1;
    temp->country_by_date = new_list_node(item);
  }else{
    temp->pop_vaccinated = 0;
    temp->country_by_date = NULL;
  }

  temp->link = NULL;

  return temp;

}

//Insert item to country hash table
void country_hash_insert(COUNTable records, char** item){
  if(records == NULL) return;

  int h = hash(item[location]);

  country_hash_list_insert(&(records[h].start), item);
}

//Insert item to hash list (same as hash.c)
void country_hash_list_insert(country_node *head, char** item){
  if(*head == NULL){
    *head = new_country_node(item);
    return;
  }else if(!strcmp((*head)->country_name, item[location])){
    country_hash_insert_at_node(*head, item);
    return;
  }
  country_node traverse = *head;

  while(traverse->link != NULL){
    if(!strcmp(traverse->country_name, item[location])){
      country_hash_insert_at_node(traverse, item);
      return;
    }

    traverse = traverse->link;
  }

  traverse->link = new_country_node(item);

}

//Same as hash_insert at node but adjusted to country hash
void country_hash_insert_at_node(country_node head, char** item){
  if(head == NULL) return;

  head->population++;

  if(!strcmp(item[yes_no], "YES")){
    head->pop_vaccinated++;
    if(head->country_skip == NULL && head->pop_vaccinated == 100){
      head->skip_height++;
      head->country_skip = malloc(sizeof(SList) * head->skip_height);
      head->country_skip[head->skip_height - 1] = new_skip_list(head->country_by_date);
      
      if(compareDate(head->country_skip[head->skip_height - 1]->skip_date, item[vac_date]) == 1){
        LNode temp = new_list_node(item);
        temp->next = head->country_by_date;
        head->country_by_date = temp;

        for(int i = 0 ; i < head->skip_height - 1 ; i++){
          if(head->country_skip[i]->level == 1){
            SList insert = new_skip_node(temp, NULL);
            insert->right = head->country_skip[i];
            head->country_skip[i] = insert;
          }else{
            SList insert = new_skip_node(NULL, head->country_skip[i-1]);
            insert->right = head->country_skip[i];
            head->country_skip[i] = insert;
          }
        }

      }else{
        country_skip_lists_search_insert(head->country_skip, head->country_by_date, head->skip_height, item);
      }
      
    }else if(head->country_skip && head->pop_vaccinated % 100 == 0){
      head->skip_height++;
      head->country_skip = realloc(head->country_skip, sizeof(SList) * head->skip_height);
      head->country_skip[head->skip_height - 1] = skip_increase_height(head->country_skip[head->skip_height - 2]);
      
      if(compareDate(head->country_skip[head->skip_height - 1]->skip_date, item[vac_date]) == 1){
        LNode temp = new_list_node(item);
        temp->next = head->country_by_date;
        head->country_by_date = temp;

        for(int i = 0 ; i < head->skip_height - 1 ; i++){
          if(head->country_skip[i]->level == 1){
            SList insert = new_skip_node(temp, NULL);
            insert->right = head->country_skip[i];
            head->country_skip[i] = insert;
          }else{
            SList insert = new_skip_node(NULL, head->country_skip[i-1]);
            insert->right = head->country_skip[i];
            head->country_skip[i] = insert;
          }
        }

      }else{
        country_skip_lists_search_insert(head->country_skip, head->country_by_date, head->skip_height, item);
      }

    }else{

      if(head->country_skip){
        if(compareDate(head->country_skip[head->skip_height - 1]->skip_date, item[vac_date]) == 1){
          LNode temp = new_list_node(item);
          temp->next = head->country_by_date;
          head->country_by_date = temp;

          for(int i = 0 ; i < head->skip_height - 1 ; i++){
            if(head->country_skip[i]->level == 1){
              SList insert = new_skip_node(temp, NULL);
              insert->right = head->country_skip[i];
              head->country_skip[i] = insert;
            }else{
              SList insert = new_skip_node(NULL, head->country_skip[i-1]);
              insert->right = head->country_skip[i];
              head->country_skip[i] = insert;
            }
          }

        }else{
          country_skip_lists_search_insert(head->country_skip, head->country_by_date, head->skip_height, item);
        }
      }else{
        country_list_insert((&(head)->country_by_date), item);
      }
    }

  }
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

  skip_lists_free(head->country_skip, head->skip_height);

  list_free(head->country_by_date);

  free(head);
}

//Find and return a hash node
country_node country_hash_search(COUNTable records, char* find_country){
  if(records == NULL) return NULL;

  int h = hash(find_country);

  if(records[h].start != NULL){
    
    country_node traverse = records[h].start;

    while(traverse){

      if(!strcmp(traverse->country_name, find_country)){
        return traverse;
      }

      traverse = traverse->link;

    }

    return NULL;

  }else{
    return NULL;
  }

}

//Find and return a list node
LNode country_hash_node_search(COUNTable records, country_node found, char* find_country, char* find_date, int flag){
  if(records == NULL) return NULL;

  country_node given_node = NULL;

  //If we already have the country hash_node
  if(found) given_node = found;
  //else search for it
  else given_node = country_hash_search(records, find_country);

  if(given_node != NULL){
    //Search a list node with the desired date and return it
    if(given_node->country_skip != NULL){
      LNode temp = country_skip_lists_search(given_node->country_skip, given_node->skip_height , find_date, flag);
      if(temp) return temp;
    }
    return country_list_node_search(given_node->country_by_date, find_date,flag);
      
  }else{
    return NULL;
  }
}

//Print statistics for all countries
void country_hash_search_print_statistics(COUNTable records, PHash country_records, char** item, int flag){
  if(records == NULL) return;

  for(int i = 0 ; i < hsize ; i++){
    if(records[i].start != NULL){

      country_node traverse = records[i].start;
      while(traverse){

        pop_ptr pop = pop_hash_search(country_records, traverse->country_name);
        country_hash_print_statistics(records, traverse, pop, traverse->country_name, item[2], item[3], flag);

        traverse = traverse->link;
      }

    }
  }
}

//Print statistics for 1 country
void country_hash_print_statistics(COUNTable records, country_node found, pop_ptr statistic, char* find_country, char* find_first_date, char* find_second_date,int flag){
  if(records == NULL) return;

  country_node given_country = NULL;

  if(found) given_country = found;
  else given_country = country_hash_search(records, find_country);

  LNode start_date = country_hash_node_search(records, found, find_country, find_first_date, 0);
  LNode end_date = country_hash_node_search(records, found, find_country, find_second_date, 1);

  //if start node doesnt exist no records exist
  if(start_date == NULL){
    if(flag){
      printf("%s\n", find_country);
      printf("0-20 0 0 0.0000%%\n");

      printf("20-40 0 0 0.0000%%\n");
    
      printf("40-60 0 0 0.0000%%\n");
    
      printf("60+ 0 0 0.0000%%\n");
    }else{
      printf("%s 0 %.2f%%\n",find_country,(double)((double)100*given_country->pop_vaccinated/(double)statistic->population));
    }
    return;
  }
  //It means that end date is smaller that the first date at the list. So no records for that time period
  if(end_date == NULL){
    if(flag){
      printf("%s\n", find_country);
      printf("0-20 0 0 0.0000%%\n");

      printf("20-40 0 0 0.0000%%\n");
    
      printf("40-60 0 0 0.0000%%\n");
    
      printf("60+ 0 0 0.0000%%\n");
    }else{
      printf("%s 0 %.2f%%\n",find_country,(double)((double)100*given_country->pop_vaccinated/(double)statistic->population));
    }
    return;
  }

  LNode traverse = end_date->next;

  //If there are more nodes with the same dates traverse them and make them the end date
  if(traverse){
    while(compareDate(traverse->data->date, end_date->data->date) == 0 || traverse->next == NULL){
      end_date = traverse;
      
      traverse = traverse->next;
    }
  }


  LNode traverse_count = start_date;

  //popStatusByAge -> flag = 1
  if(flag){

    int total_20 = 0;
    int total_40 = 0;
    int total_60 = 0;
    int total_60plus = 0;

    while(traverse_count != end_date){
      if(traverse_count->data->age <= 20){
        total_20++;
      }else if(traverse_count->data->age <= 40){
        total_40++;
      }else if(traverse_count->data->age <= 60){
        total_60++;
      }else{
        total_60plus++;
      }

      traverse_count = traverse_count->next;
    }

    printf("%s\n", find_country);
    if(statistic->pop_total20)
      printf("0-20 %d %d %.4f%%\n",total_20, statistic->pop_total20, (double)((double)100*total_20/(double)statistic->pop_total20));
    else
      printf("0-20 0 0 0.0000%%\n");
    
    if(statistic->pop_total40)
      printf("20-40 %d %d %.4f%%\n",total_40, statistic->pop_total40, (double)((double)100*total_40/(double)statistic->pop_total40));
    else
      printf("20-40 0 0 0.0000%%\n");
    
    if(statistic->pop_total60)
      printf("40-60 %d %d %.4f%%\n",total_60, statistic->pop_total60, (double)((double)100*total_60/(double)statistic->pop_total60));
    else
      printf("40-60 0 0 0.0000%%\n");
    
    if(statistic->pop_total60plus)
      printf("60+ %d %d %.4f%%\n",total_60plus, statistic->pop_total60plus, (double)((double)100*total_60plus/(double)statistic->pop_total60plus));
    else
      printf("60+ 0 0 0.0000%%\n");

  }else{
    
    int total = 0;

    //count the nodes from start date to end date
    while(traverse_count != end_date){
      total++;

      traverse_count = traverse_count->next;
    }

    printf("%s %d %.2f%%\n",find_country, total ,(double)((double)100*given_country->pop_vaccinated/(double)statistic->population));
  }
}

//Print the hash table(NO use currently)
void country_hash_print(COUNTable records){
  if(records == NULL) return;

  for(int i = 0 ; i < hsize ; i++){
    if(records[i].start != NULL) country_hash_list_print(records[i].start);
  }
}

//Print the hash lists(NO use currenlty)
void country_hash_list_print(country_node head){
  if(head == NULL) return;

  country_node traverse = head;

  while(traverse){
    printf("country name = %s %p\n",traverse->country_name,traverse);

    skip_lists_print(traverse->country_skip, traverse->skip_height);
    list_print(traverse->country_by_date);

    traverse = traverse->link;
  }
}