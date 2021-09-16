#ifndef COUNTRYHASH_H
#define COUNTRYHASH_H

#include "list.h"
#include "population.h"

typedef struct country_hashlist{
  SList *country_skip;
  LNode country_by_date;
  int skip_height;
  char* country_name;
  int population;
  int pop_vaccinated;
  struct country_hashlist* link;
}country_hashlist, *country_node;

typedef struct country_hash{
  country_node start;
}country_hash, *COUNTable;

COUNTable country_hash_initialize(void);
country_node new_country_node(char** item);
void country_hash_insert(COUNTable records, char** item);
void country_hash_list_insert(country_node *head, char** item);
void country_hash_insert_at_node(country_node head, char** item);
void country_hash_initialize_skip_lists(COUNTable records);
void country_hash_free(COUNTable records);
void country_hash_list_free(country_node head);
void country_hash_print(COUNTable records);
void country_hash_list_print(country_node head);
void country_hash_search_print_statistics(COUNTable records, PHash country_records, char** item, int flag);
void country_hash_print_statistics(COUNTable records, country_node found, pop_ptr statistic, char* find_country, char* find_first_date, char* find_second_date, int flag);
LNode country_hash_node_search(COUNTable records, country_node found, char* find_country, char* find_date, int flag);
country_node country_hash_search(COUNTable records, char* find_country);

#endif