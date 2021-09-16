#ifndef CITIZENHASH_H
#define CITIZENHASH_H

#include "list.h"

typedef struct citizen_info{
  char* firstName;
  char* lastName;
  char* country;
  int age;
}citizen_info, *check_info;

typedef struct citizen_hashlist{
  char* cit_id;
  check_info cit_data;
  struct citizen_hashlist* next;
}citizen_hashlist, *CHNode;


typedef struct citizen_hash{
  CHNode start;
}citizen_hash, *CHTable;

CHTable citizen_hash_initialize(void);
check_info new_citizen_info(char** item);
CHNode new_citizen_node(char** item);
void citizen_hash_insert(CHTable records, char** item);
int citizen_hash_search(CHTable records, char** item);
int citizen_hash_search_id(CHTable records, char* item);
check_info citizen_hash_return_info(CHTable records, char* item);
int compare_record(check_info cit_data, char** item);
void citizen_hash_free(CHTable citizen_records);
void citizen_hashlist_free(CHNode head);
void citizen_info_free(check_info data);

#endif