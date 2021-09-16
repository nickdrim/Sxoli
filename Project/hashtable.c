#include "header.h"
#include "time.h"

float average_tf_idf = 0;
int total_words = 0;
entry_w** largest_tf = NULL;
#define TABLE_SIZE 100000

int vector_size(void){
	return 1000;
}

void free_global_array(void){
	free(largest_tf);
}

void ht_calculate_tf_idf(ht_w* ht){
	if(ht == NULL) return;

	float sum_tf_idf = 0;
	int sum_words = 0;

	for(int i = 0 ; i < TABLE_SIZE ; i++){
		if(ht->entries[i] != NULL){

			entry_w* traverse = ht->entries[i];
			while(traverse != NULL){
				tf_calc(traverse);
				if(traverse->idf > 0){
					sum_words++;
					traverse->tf = traverse->tf / traverse->count_different_json;
					traverse->tf_idf = traverse->tf * traverse->idf;
					sum_tf_idf += traverse->tf_idf;
				}

				traverse = traverse->next;
			}
		}
	}

	average_tf_idf = sum_tf_idf / sum_words;
}

entry_w* ht_insert(ht_w* ht, char* temp,char* fullpath, int current){
	if(ht == NULL) return NULL;

	unsigned int slot = hashword(temp, 0);

	if(ht->entries[slot] == NULL){
		ht->entries[slot] = create_entry(temp,fullpath);
		return ht->entries[slot];
	}else{
		entry_w* traverse = ht->entries[slot];
		if(traverse->next != NULL){
			while(traverse->next != NULL){
				if(!strcmp(traverse->word,temp)){
					update_entry(traverse, current, 0);
					return NULL;
				}else{
					traverse = traverse->next;
				}
			}
		}else{
			if(!strcmp(traverse->word,temp)){
				update_entry(traverse, current, 0);
				return NULL;
			}
		}

		return (traverse->next = create_entry(temp,fullpath));
	}
}

void tf_calc(entry_w* data){
	if(data != NULL);
	data->tf = data->count_word / data->count_different_word;
	data->idf = log(29787/data->count_word);
}

void update_entry(entry_w* data,int current,int total){
	if(data == NULL) return;
	if(!total){
		data->count_word++;
		if(data->count_different_json != current){
			data->count_different_json++;
		}
	}else{
		data->count_different_word += total;
	}
}

entry_w* create_entry(char* temp, char* fullpath){
	entry_w* entry = malloc(sizeof(entry_w));
	entry->word=malloc(strlen(temp)+1);
	strcpy(entry->word,temp);
	entry->path=malloc(strlen(fullpath)+1);
	strcpy(entry->path,fullpath);
	entry->count_word=1;
	entry->count_different_json=1;
	entry->count_different_word = 0;
	entry->index=-1;
	entry->tf = 0;
	entry->tf_idf = 0;
	entry->idf = 0;
	entry->next=NULL;
	return entry;
}

void quicksort(entry_w** largest,int first,int last){
   int i, j, pivot;
	entry_w* temp;

   if(first<last){
      pivot=first;
      i=first;
      j=last;

      while(i<j){
         while(largest[i]->tf_idf >=largest[pivot]->tf_idf && i<last)
            i++;
         while(largest[j]->tf_idf<largest[pivot]->tf_idf)
            j--;
         if(i<j){
            temp=largest[i];
            largest[i] = largest[j];
            largest[j] = temp;
         }
      }

      temp = largest[pivot];
      largest[pivot] = largest[j];
      largest[j] = temp;
      quicksort(largest,first,j-1);
      quicksort(largest,j+1,last);

   }
}

int hashword(char *key, int size){
	if(size == 0) size = TABLE_SIZE;

	unsigned int  h=0;
	int i;
	for(i=0;i<strlen(key);i++){
		h=(h<<5 | h>>27);
		h+=(unsigned int)key[i];
	}
	h= h % size;
	return h;
}

ht_w *ht_create(int size){
	//allocate table and its entries
	if(size == 0) size = TABLE_SIZE;

	ht_w *hashtable = malloc(sizeof(ht_w));
	hashtable->entries = malloc(sizeof(entry_w*)* size);

	for(int i=0; i < size ;i++){
		hashtable->entries[i] = NULL;
	}
	return hashtable;
}

ht_w* create_smaller_ht(ht_w *ht){
	if(ht == NULL) return NULL;

	ht_w* new_ht = ht_create(10000);

	for(int i=0 ; i < TABLE_SIZE ; i++){
		if(ht->entries[i] != NULL){
			entry_w* traverse = ht->entries[i];
			while(traverse != NULL){
				if(traverse->tf_idf >= average_tf_idf){
					
					if(largest_tf == NULL){
						largest_tf = malloc(sizeof(entry_w *));
						largest_tf[0] = traverse;
						total_words++;
					}else{
						total_words++;
						largest_tf = realloc(largest_tf,sizeof(entry_w *) * total_words);
						largest_tf[total_words - 1] = traverse;
					}
					
				}
				
				traverse = traverse->next;
				
			}
		}
	}

	quicksort(largest_tf,0,total_words-1);

	if(total_words > 1000) total_words = 1000;
	for(int i = 0 ; i < total_words ; i++){
		ht_entry_insert(new_ht, largest_tf[i], i);
	}

	free_global_array();
	ht_free(ht, 0);
	return new_ht;
}

void ht_entry_insert(ht_w *ht, entry_w* data, int index){

	unsigned int slot = hashword(data->word,10000);

	if(ht->entries[slot] == NULL){
		ht->entries[slot] = copy_entry(data, index);
	}else{
		entry_w *traverse = ht->entries[slot];
		while(traverse->next!=NULL){
			traverse = traverse->next;
		}
		traverse->next = copy_entry(data, index);
	}
}

entry_w* copy_entry(entry_w* data, int number){
	
	entry_w *temp = malloc(sizeof(entry_w));
	
	temp->word = malloc(strlen(data->word) + 1);
	strcpy(temp->word,data->word);
	temp->path = malloc(strlen(data->path) + 1);
	strcpy(temp->path,data->path);
	temp->count_different_json = data->count_different_json;
	temp->count_word = data->count_word;
	temp->tf = data->tf;
	temp->idf = data->idf;
	temp->tf_idf = data->tf_idf;
	temp->next = NULL;
	temp->index = number;

	return temp;
}

void ht_new_print(ht_w* ht, int size){
	
	printf("Average tf_idf = %f\n", average_tf_idf);

	for(int i = 0 ; i < size ; i++){
		if(ht->entries[i] != NULL){
			printf("slot[%4d]: ", i);

			entry_w *temp = ht->entries[i];

			while(temp != NULL){
				printf("%s %f %f %f\t",temp->word, temp->tf, temp->idf, temp->tf_idf);
				temp = temp->next;
			}
			printf("\n");
		}
		
	}
}

int ht_search(ht_w* ht, char* string){
	if(ht == NULL) return 0;

	int slot = hashword(string, 10000);
	entry_w* temp = ht->entries[slot];

	while(temp != NULL){
		if(strcmp(temp->word,string) == 0){
			return temp->index;
		}
		temp = temp->next;
	}

	return -1;
	
}

void ht_free(ht_w *ht,int size){
	if(ht == NULL) return;

	if(size == 0) size = TABLE_SIZE;

	for(int i = 0; i < size ; i++){
		if(ht->entries[i] != NULL){
			ht_entry_free(ht->entries[i]);
		}
	}

	free(ht->entries);
	free(ht);
}

void ht_entry_free(entry_w *head){
	if(head == NULL) return;
	
	free(head->word);
	free(head->path);
	ht_entry_free(head->next);
	free(head);
}
