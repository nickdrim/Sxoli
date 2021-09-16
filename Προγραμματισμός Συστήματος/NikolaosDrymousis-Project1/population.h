#ifndef POPULATION_H
#define POPULATION_H

typedef struct population{
  char* country_name;
  int population;
  int pop_total20;
  int pop_total40;
  int pop_total60;
  int pop_total60plus;
  struct population* link;
}population,*pop_ptr;

typedef struct pop_hash{
  pop_ptr start;
}pop_hash, *PHash;

PHash pop_hash_initialize(void);
void pop_hash_insert(PHash records, char* item, int age);
pop_ptr pop_hash_search(PHash records, char* item);
void pop_hash_list_insert(pop_ptr* head, char* item, int age);
pop_ptr new_pop_node(char* item, int age);
void pop_hash_free(PHash records);
void pop_hash_list_free(pop_ptr head);

#endif