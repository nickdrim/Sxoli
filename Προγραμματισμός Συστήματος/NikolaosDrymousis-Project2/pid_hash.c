#include "header.h"

int psize = 100;

//Initializes a pid hash table
PIDHASH pid_hash_initialize(void){

  PIDHASH temp = malloc(sizeof(pid_hash) * psize);

  for(int i = 0 ; i < psize ; i++){
    temp[i].start = NULL;
  }

  return temp;
}

//Creates and returns a new pid node
pid_ptr new_pid_node(int pid, char* w_fifo, char* r_fifo, int index){

  pid_ptr temp = malloc(sizeof(pid_info));

  temp->countries = country_hash_initialize();
  temp->dirs = NULL;
  temp->wfile = w_fifo;
  temp->rfile = r_fifo;
  temp->process_id = pid;
  temp->dir_size = 0;
  temp->arr_index = index;
  temp->next = NULL;

  return temp;

}

//Inserts a new pid to the hash table
void pid_hash_insert(PIDHASH records, int pid, char* w_fifo, char* r_fifo, int index){
  if(records == NULL) return;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf, psize);

  pid_hash_list_insert(&(records[h].start), pid, w_fifo, r_fifo, index);
}

//Inserts a new pid in the list of the hash node
void pid_hash_list_insert(pid_ptr* head, int pid, char* w_fifo, char* r_fifo, int index){
  if(*head == NULL){
    *head = new_pid_node(pid, w_fifo, r_fifo, index);
  }else{
    pid_ptr traverse = *head;

    while(traverse->next){
      traverse = traverse->next;
    }

    traverse->next = new_pid_node(pid, w_fifo, r_fifo, index);
  }
}


void pid_hash_search_insert(PIDHASH records, int pid, char* dname, char* country){
  if(records == NULL) return;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf,psize);

  pid_hash_list_search_insert(records[h].start, pid, dname, country);
}

//Locates the node that stores pid and adds the directories and countries that it is
//responsible for
void pid_hash_list_search_insert(pid_ptr head, int pid, char* dname, char* country){
  if(head == NULL) return;

  pid_ptr traverse = head;

  while(traverse){
    if(traverse->process_id == pid){

      traverse->dir_size++;
      if(traverse->dirs == NULL){
        traverse->dirs = malloc(sizeof(char *) * traverse->dir_size);
        traverse->dirs[0] = malloc(strlen(dname) + 1);
        strcpy(traverse->dirs[0], dname);
      }else{
        traverse->dirs = realloc(traverse->dirs,sizeof(char *) * traverse->dir_size);
        traverse->dirs[traverse->dir_size-1] = malloc(strlen(dname) + 1);
        strcpy(traverse->dirs[traverse->dir_size-1], dname);
      }

      country_hash_insert(traverse->countries, country);

      break;
    }

    traverse = traverse->next;
  }
}
 
//In case of SIGCHLD locate the id of the terminated child in the hash table
//and copy its containts to another node with id equal to new_pid
int pid_hash_update(PIDHASH records, int prev_pid, int new_pid){
  if(records == NULL) return -1;

  char buf[20];
  sprintf(buf, "%d", prev_pid);

  int h = hash(buf,psize);

  pid_ptr temp = pid_hash_list_replace(&(records[h].start), prev_pid);

  int index = temp->arr_index;

  sprintf(buf, "%d", new_pid);

  h = hash(buf,psize);

  pid_ptr traverse = records[h].start;

  if(traverse == NULL){
    records[h].start = pid_hash_replace(temp, new_pid);
    pid_hash_list_node_free(temp);
  }else{
    while(traverse->next){
      traverse = traverse->next;
    }

    traverse->next = pid_hash_replace(temp,new_pid);
    pid_hash_list_node_free(temp);
  }

  return index;
}

//Locate the id of the terminated child in the list of hash node,
//remove it from the list and return it
pid_ptr pid_hash_list_replace(pid_ptr *head, int prev_id){
  if(*head == NULL) return NULL;

  pid_ptr temp = NULL;

  if((*head)->process_id == prev_id){
    temp = *head;
    *head = (*head)->next;
    temp->next = NULL;
    return temp;
  }else{

    pid_ptr previous = *head;
    pid_ptr traverse = (*head)->next;

    while(traverse){
      if(traverse->process_id == prev_id){
        previous->next = traverse->next;
        traverse->next = NULL;
        return traverse;
      }
      
      previous = traverse;
      traverse = traverse->next;

    }

    return traverse;

  }
}

//Copies the contains of the previous hash node into a new
//node with id = new_id
pid_ptr pid_hash_replace(pid_ptr node, int new_id){
  if(node == NULL) return NULL;

  pid_ptr temp = malloc(sizeof(pid_info));
  temp->countries = node->countries;
  temp->process_id = new_id;
  temp->dir_size = node->dir_size;
  temp->arr_index = node->arr_index;
  temp->dirs = malloc(sizeof(char *) * temp->dir_size);
  temp->next = NULL;
  temp->wfile = node->wfile;
  temp->rfile = node->rfile;

  for(int i = 0 ; i < temp->dir_size ; i++){
    temp->dirs[i] = malloc(strlen(node->dirs[i]) + 1);
    strcpy(temp->dirs[i],node->dirs[i]);
  }

  return temp;

}

//Returns the fifo file of the pid for reading
char* pid_hash_return_read_fifo(PIDHASH records, int pid){
  if(records == NULL) return NULL;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf,psize);

  pid_ptr traverse = records[h].start;

  while(traverse){
    if(traverse->process_id == pid){
      return traverse->rfile;
    }

    traverse = traverse->next;
  }

  return NULL;
  
}

//Initializes all bloom filters of all countries that pid responsible for,
//for virus = desease
void pid_hash_initialize_bloom(PIDHASH records, int pid, char* desease, int* temp_bloom){
  if(records == NULL) return;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf,psize);

  pid_ptr traverse = records[h].start;

  while(traverse){
    if(traverse->process_id == pid){
      country_hash_initialize_bloom(traverse->countries, desease, temp_bloom); 
      return;
    }

    traverse = traverse->next;
  }
}

//Checks if citizen with citizen_id from specific country is vaccinated against desease
//and return the index that the pid is stored in the global array of travelMonitor
int pid_hash_check_bloom(PIDHASH records, int pid, char* virus, char* citizen_id, char* country, char* date){
  if(records == NULL) return 0;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf,psize);

  pid_ptr traverse = records[h].start;

  while(traverse){
    if(traverse->process_id == pid){
      if(country_hash_bloom_check(traverse->countries, virus, citizen_id, country, date)){
        return traverse->arr_index;
      }else{
        return -1;
      }
    }

    traverse = traverse->next;
  }

  return -1;
}

//Returns the index of the global array of travelMonitor in which
//pid is stored(Used in case of a SIGCHLD) so that it will be replaced
int pid_hash_return_index(PIDHASH records, int pid){
  if(records == NULL) return -1;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf,psize);

  pid_ptr traverse = records[h].start;

  while(traverse){
    if(traverse->process_id == pid){
      return traverse->arr_index;
    }

    traverse = traverse->next;
  }

  return 0;
}

//Updates the bloom filter of a desease for all countries that pid is
//responsible for
void pid_hash_update_bloom(PIDHASH records, int pid, char* desease, int* new_bloom){
  if(records == NULL) return;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf,psize);

  pid_ptr traverse = records[h].start;

  while(traverse){
    if(traverse->process_id == pid){
      country_hash_update_bloom(traverse->countries, desease, new_bloom);
      return;
    }

    traverse = traverse->next;
  }


}

//Locates and returns the write fifo file that pid is responsible for
char* pid_hash_return_write_fifo(PIDHASH records, int pid){
  if(records == NULL) return NULL;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf,psize);

  pid_ptr traverse = records[h].start;

  while(traverse){
    if(traverse->process_id == pid){
      return traverse->wfile;
    }

    traverse = traverse->next;
  }

  return NULL;
  
}

//Creates a new travel request for a specific country
void pid_new_request(PIDHASH records, int pid, char* country, char* desease, char* new_date, int answer){
  if(records == NULL) return;

  char buf[20];
  sprintf(buf, "%d", pid);

  int h = hash(buf, psize);

  pid_ptr traverse = records[h].start;

  while(traverse){
    if(traverse->process_id == pid){
      return country_new_request(traverse->countries, country, desease, new_date, answer);
    }

    traverse = traverse->next;
  }

  return;
}

//Prints all requests made between to dates for either a country or all of the countries
//pid is responsible for according to if country is NULL or not
void pid_print_requests(PIDHASH records, char* desease, char* start_date, char* end_date, char* country, int pid){
  if(records == NULL) return;

  if(country != NULL){
    char buf[20];
    sprintf(buf, "%d", pid);

    int h = hash(buf, psize);

    pid_ptr traverse = records[h].start;
    
    while(traverse){
      if(traverse->process_id == pid){
        country_print_requests(traverse->countries, desease, start_date, end_date, country);
        return;
      }

      traverse = traverse->next;
    }

  }else{
    //Place all countries that a pid is responsible into one array
    COUNTable* countries = NULL;
    int countries_size = 0;

    for(int i = 0 ; i < psize ; i++){
      if(records[i].start != NULL){
        pid_ptr traverse = records[i].start;

        while(traverse){
          if(countries == NULL){
            countries_size++;
            countries = malloc(sizeof(COUNTable) * countries_size);
            countries[0] = traverse->countries;
          }else{
            countries_size++;
            countries = realloc(countries, sizeof(COUNTable) * countries_size);
            countries[countries_size - 1] = traverse->countries;
          }
          traverse = traverse->next;
        }

      }
    }

    countries_print_requests(countries, countries_size,  desease, start_date, end_date);

  }
}

//Frees a pid hash table
void pid_hash_free(PIDHASH records){
  if(records == NULL) return;

  for(int i = 0 ; i < psize ; i++){
    pid_hash_list_free(records[i].start);
  }

  free(records);
}

//Frees a pid hash list
void pid_hash_list_free(pid_ptr head){
  if(head == NULL) return;

  pid_hash_list_free(head->next);


  country_hash_free(head->countries);

  for(int i = 0 ; i < head->dir_size ; i++){
    free(head->dirs[i]);
  }
  free(head->dirs);

  free(head);

}

//Frees the contains of pid hash list node
void pid_hash_list_node_free(pid_ptr head){
  if(head == NULL) return;

  head->countries = NULL;

  for(int i = 0 ; i < head->dir_size ; i++){
    free(head->dirs[i]);
  }
  free(head->dirs);
  head->wfile = NULL;
  head->rfile = NULL;

  free(head);

}

//Catalog functions

//Locates the country in a catalog and returns the pid of the process
//that is responsible for it
int return_catalog_pid(catalog* countries, int size, char* country){
  if(countries == NULL) return -1;

  for(int i = 0 ; i < size ; i++){
    if(!strcmp(countries[i]->country,country)){
      return countries[i]->pid;
    }
  }

  return -1;
}

//Returns only the names of the countries in the catalog
char** return_catalog_countries(catalog* countries, int size){
  if(countries == NULL) return NULL;

  char** temp = malloc(sizeof(char*) * size);

  for(int i = 0 ; i < size ; i++){
    temp[i] = malloc(strlen(countries[i]->country) + 1);
    strcpy(temp[i], countries[i]->country);
  }

  return temp;
}

void change_catalog_pid(catalog* countries, int size, int old_id, int new_id){
  if(countries == NULL) return;

  for(int i = 0 ; i < size ; i++){
    if(countries[i]->pid == old_id){
      countries[i]->pid = new_id;
    }
  }
}

//Frees the catalog
void free_catalog(catalog* countries, int size){
  if(*countries == NULL) return;

  for(int i = 0 ; i < size ; i++){
    if(countries[i] != NULL){
      free(countries[i]->country);
      free(countries[i]);
    }
  }

  free(countries);
}