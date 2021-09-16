#ifndef HASHTABLE_H
#define HASHTABLE_H


typedef struct entry_w{
	char *word;
	char *path;
	int count_word;
	int count_different_json;
	int count_different_word;
	float tf;
	float idf;
	float tf_idf;
	int index;
	struct entry_w *next;
}entry_w;

typedef struct{
	entry_w **entries;
} ht_w;

int hashword(char *key, int size);
ht_w *ht_create(int size);
entry_w* ht_insert(ht_w *ht, char* temp,char* fullpath,int current);
void ht_entry_free(entry_w *head);
void ht_free(ht_w *ht,int size);
ht_w *create_smaller_ht(ht_w *ht);
void ht_entry_insert(ht_w* ht, entry_w* data, int index);
entry_w* copy_entry(entry_w* data, int number);
int ht_search(ht_w* ht, char* string);
void ht_new_print(ht_w* ht,int size);
void tf_calc(entry_w* data);
void ht_calculate_tf_idf(ht_w* ht);

int vector_size(void);
void free_global_array(void);
void quicksort(entry_w** largest,int first,int last);

entry_w* create_entry(char* temp, char* fullpath);
void update_entry(entry_w* data,int current, int total);
#endif //HASHTABLE_H
