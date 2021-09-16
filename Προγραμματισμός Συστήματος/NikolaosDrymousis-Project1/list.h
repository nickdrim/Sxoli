#ifndef LIST_H
#define LIST_H

typedef struct info{
  int citizenID;
  char* firstName;
  char* lastName;
  char* country;
  char* virusName;
  char* vaccine;
  char* date;
  int age;
}info,*info_ptr;


typedef struct bloomfilter{
  int* filter;
  int filter_size;
}bloomfilter, *BFilter;

typedef struct list{
  info_ptr data;
  int level;
  struct list* next;
}list, *LNode;


typedef struct skip_list{
  int skip_id;
  int level;
  char* skip_date;
  struct skip_list* right;
  struct skip_list* lower;
  struct skip_list* above;
  LNode node;
}skip_list, *SList;

unsigned int Log2n(unsigned int n);

//List functions
info_ptr new_info(char** item);
LNode new_list_node(char** item);
void list_insert(LNode *head, char** item);
int list_search(LNode head, int item);
LNode list_node_search(LNode head, int item);
int list_node_search_insert(SList top, LNode head, char** item);
void list_print(LNode head);
int list_delete(LNode head, int target);
void list_free(LNode head);
void info_free(info_ptr item);

//Bloom filter functions
BFilter new_bloom_filter(int size);
int bloom_filter_check(BFilter check, char* item);
void bloom_filter_update(BFilter check, char* item);
void bloom_filter_free(BFilter check);


//Skip lists functions
SList new_skip_node(LNode item, SList below);
SList new_skip_list(LNode head);
SList skip_increase_height(SList item);
LNode skip_lists_search(SList* head, int height, int item);
LNode skip_list_search(SList head, int item);
void skip_lists_search_insert(SList* levels, LNode head, int height, char** item);
int skip_list_search_insert(SList head, char** item);
void skip_lists_print(SList* head, int height);
void skip_list_print(SList head);
void skip_list_delete_below(SList previous, SList current);
int skip_list_search_delete(SList head, int item);
int skip_lists_search_delete(SList* head, int height, int item);
void skip_list_below(SList head);
void skip_lists_free(SList* head, int height);
void skip_list_free(SList head);

//Country skip lists functions
int country_skip_lists_search_delete(SList* head, int height, char* item);
int country_skip_list_search_delete(SList head, char* item);
LNode country_skip_lists_search(SList* head, int height, char* item, int flag);
LNode country_skip_list_search(SList head, char* item,int flag);
void country_skip_lists_search_insert(SList* levels, LNode head, int height, char** item);
int country_skip_list_search_insert(SList head, char** item);
void country_list_insert(LNode *head, char** item);
int country_list_node_search_insert(SList top, LNode head, char** item);
LNode country_list_node_search(LNode head, char* item, int flag);
int country_list_delete(LNode head, char* target);

#endif