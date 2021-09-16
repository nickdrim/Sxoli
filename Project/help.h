#ifndef HELP_H
#define HELP_H

#include "list.h"

typedef struct weight{
  double b;
  double *w;
}weight,*weight_ptr;

typedef struct set{
  char* string1;
  char* string2;
  int expected;
}set, *set_ptr;

typedef struct full_set{
  set_ptr* training_set;
  set_ptr* test_set;
  set_ptr* validation_set;
  int training_size;
  int test_size;
  int validation_size;
}full_set,*full_set_ptr;

typedef struct vectorized_set{
  char* string1;
  char* string2;
  int* x_vector;
  int value;
}vectorized_set, *vset_ptr;

typedef struct batch{
 vset_ptr* batches;
 int batch_size;
 ht_w* words;
 int successfull;
}batch,*batch_pointer;

void negative_relationship(pbucket head, pbucket data);
void create_negative_relationships(HTable records, full_set_ptr w_set);
void count_and_print_negative(CNode list);


double first_test(int *x_vector);
void test_set(batch_pointer test);
int test(int *x_vector, int y);
int validate(int *x_vector);

void train_test_validate(full_set_ptr w_set, ht_w* word_hash);
void train(batch_pointer test);
void first_training(full_set_ptr w_set, ht_w* word_hash, HTable records);
int first_train(int *x_vector, int y);

int* sparse_array(int* vector, int* index_size,int* total_indexes,int* total_index_size,int flag);
int* initialize_index_vector(void);

weight_ptr create_weights(void);
void free_weights(weight_ptr temp);
void resolve_transitivity(HTable records, full_set_ptr w_set, set_ptr* new_train, int size);

set_ptr create_set(char* item1, char* item2, int val);
void set_free(set_ptr* free_set, int size);
full_set_ptr create_full_set(int size);
void free_full_set(full_set_ptr w_set);
batch_pointer create_batch(int size_of_batch, ht_w* word_hash);
vset_ptr create_vectorized_set(int current,set_ptr* cur_set, ht_w* word_hash);
void batch_free(batch_pointer* w_set, int size);

void wprint(full_set_ptr w_set);
void print_clique_array(pbucket *visited);

#endif