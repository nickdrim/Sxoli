#ifndef COUNTRYHASH_H
#define COUNTRYHASH_H

#include "list.h"

typedef struct apply{
  char* date;
  int answer;
  struct apply* next;
}apply,*ANode;


typedef struct virus{
  char* virus_name;
  BFilter bloom;
  ANode req;
  struct virus* right;
}virus, *VNode;

typedef struct country_hashlist{
  char* country_name;
  VNode virus_list;
  struct country_hashlist* link;
}country_hashlist, *country_node;

typedef struct country_hash{
  country_node start;
}country_hash, *COUNTable;

COUNTable country_hash_initialize(void);
void country_hash_insert(COUNTable records, char* item);
void country_hash_list_insert(country_node *head, char* item);
void country_hash_search_insert(COUNTable records, char* find_country, char* desease, BFilter new_bloom);
void country_hash_list_search_insert(country_node head, char* find_country, char* desease, BFilter new_bloom);
int country_hash_bloom_check(COUNTable records, char* virus, char* citizen_id, char* country, char* date);
void country_hash_initialize_bloom(COUNTable records, char* desease, int* temp_bloom);
void country_hash_free(COUNTable records);
void country_hash_list_free(country_node head);
void country_new_request(COUNTable records, char* country, char* desease, char* new_date, int answer);
void country_print_requests(COUNTable records, char* desease, char* start_date, char* end_date, char* country);
void country_hash_update_bloom(COUNTable records, char* desease, int* new_bloom);
void countries_print_requests(COUNTable* records, int record_size, char* desease, char* start_date, char* end_date);


VNode create_new_virus_node(char* desease, int* new_bloom);
void virus_list_insert(VNode head, char* desease, int* new_bloom);
int virus_list_check_bloom(VNode head, char* desease, char* citizen_id, char* date);
void virus_new_request(VNode head, char* desease, char* new_date, int answer);
void virus_print_requests(VNode head, char* desease, char* start_date, char* end_date);
void virus_list_free(VNode head);
void virus_bloom_update(VNode head, char* desease, int* new_bloom);

ANode create_new_apply_node(char* new_date, int response);
void apply_list_insert(ANode* head, char* new_date, int response);
void apply_list_print_requests(ANode head, char* start_date, char* end_date);
void apply_list_free(ANode head);

#endif