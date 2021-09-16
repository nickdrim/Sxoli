#ifndef PIDHASH_H
#define PIDHASH_H

#include "country_hash.h"

typedef struct pid_info{
  int process_id;
  COUNTable countries;
  int dir_size;
  char** dirs;
  char* wfile;
  char* rfile;
  int arr_index;
  struct pid_info* next;
}pid_info, *pid_ptr;

typedef struct pid_hash{
  pid_ptr start;
}pid_hash, *PIDHASH;

typedef struct country_catalog{
  char* country;
  int pid;
}country_catalog,*catalog;

PIDHASH pid_hash_initialize(void);
pid_ptr new_pid_node(int pid, char* w_fifo, char* r_fifo, int index);
void pid_hash_insert(PIDHASH records, int pid, char* w_fifo, char* r_fifo, int index);
void pid_hash_list_insert(pid_ptr* head, int pid, char* w_fifo, char* r_fifo,int index);
void pid_hash_search_insert(PIDHASH records, int pid, char* dname, char* country);
void pid_hash_list_search_insert(pid_ptr head, int pid, char* dname, char* country);
void pid_hash_free(PIDHASH records);
void pid_hash_list_node_free(pid_ptr head);
void pid_hash_list_free(pid_ptr head);
char** pid_hash_return_countries(PIDHASH records, int *size);
int pid_hash_update(PIDHASH records, int prev_id, int new_id);
pid_ptr pid_hash_list_replace(pid_ptr* head, int prev_id);
pid_ptr pid_hash_replace(pid_ptr node, int new_id);
char* pid_hash_return_read_fifo(PIDHASH records, int pid);
char* pid_hash_return_write_fifo(PIDHASH records, int pid);
int pid_hash_check_bloom(PIDHASH records, int pid, char* virus, char* citizen_id, char* country, char* date);
void pid_new_request(PIDHASH records, int pid, char* country, char* desease, char* new_date, int answer);
void pid_print_requests(PIDHASH records, char* desease, char* start_date, char* end_date, char* country, int pid);
void pid_hash_initialize_bloom(PIDHASH records, int pid, char* desease, int* temp_bloom);
void pid_hash_update_bloom(PIDHASH records, int pid, char* desease, int* new_bloom);
int pid_hash_return_index(PIDHASH records, int pid);

int return_catalog_pid(catalog* countries, int size, char* country);
char** return_catalog_countries(catalog* countries, int size);
void change_catalog_pid(catalog* countries, int size, int old_id, int new_id);
void free_catalog(catalog* countries, int size);

#endif