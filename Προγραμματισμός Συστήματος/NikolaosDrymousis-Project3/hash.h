#ifndef HASH_H
#define HASH_H

#include "list.h"

typedef struct hashlist{
  char* disease;
  BFilter vaccinated;
  BFilter unvaccinated;
  SList* vaccinated_levels;
  SList* unvaccinated_levels;
  int vaccinated_height;
  int unvaccinated_height;
  int no_vaccinated;
  int no_unvaccinated;
  LNode vaccinated_list;
  LNode unvaccinated_list;
  struct hashlist* link;
}hashlist, *HNode;

typedef struct hashtable{
  HNode start;
}hashtable, *HTable;

int get_hashtable_size(void);
int get_bloom_size(void);
HTable hash_initialize(int bloom);
void hash_insert(HTable records, char** info);
void hash_list_insert(HNode *head, char** item);
void hash_insert_at_node(HNode head, char** item);
void hash_initialize_skip_lists(HTable records);
HNode new_hash_node(char** item);
void hash_free(HTable records);
void hash_list_free(HNode head);
HNode hash_search(HTable records, char* x);

unsigned long djb2(unsigned char *str);
unsigned long sdbm(unsigned char *str);
unsigned long hash_i(unsigned char *str, unsigned int i);

#endif