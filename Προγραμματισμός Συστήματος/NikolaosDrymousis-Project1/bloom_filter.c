#include "header.h"

//creates and returns a bloom filter
BFilter new_bloom_filter(int size){

  BFilter temp = malloc(sizeof(bloomfilter));

  temp->filter = malloc(sizeof(int) * size);
  temp->filter_size = size;

  for(int i = 0 ; i < size ; i++){
    temp->filter[i] = 0;
  }

  return temp;

}

//Checks to see if item is possibly included in a set
//Returns 0 if it is not and 1 if it is possibly included
int bloom_filter_check(BFilter check, char* item){

  for(int i = 0 ; i < K ; i++){
    int h = hash_i((unsigned char *)item, i) % check->filter_size;
    if(!check->filter[h]) return 0;
  }

  return 1;

}

//Updates the indexes of bloom filter pointed by the item
void bloom_filter_update(BFilter check, char* item){

  for(int i = 0 ; i < K ; i++){
    int h = hash_i((unsigned char *)item, i) % check->filter_size;

    check->filter[h] = 1;
  }

}

//Deallocates memory taken from bloom filter
void bloom_filter_free(BFilter check){
  free(check->filter);
  free(check);
}

//hash function
unsigned long djb2(unsigned char *str){
	int c;
	unsigned long hash = 5381;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; // hash * 33 + c

	return hash;
}

//hash function
unsigned long sdbm(unsigned char *str){
	int c;
	unsigned long hash = 0;

	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

//hash function
unsigned long hash_i(unsigned char *str, unsigned int i){
	return djb2(str) + i * sdbm(str) + i * i;
}