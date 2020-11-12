#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashTable.h"

//Global Variables so that we dont have to pass them every time as arguments when we need them

int itemArraySize;
int sizeDisease;
int sizeCountries;

//Initializes hash table and global variables
HTable hash_initialize(int size , int BucketSize){

	static int x=1;
	if(x){
		itemArraySize = (BucketSize - sizeof(bucket *) - sizeof(int)) / sizeof(pbucket);
		//printf("itemArraySize = %d\n",itemArraySize );
		sizeDisease = size;
		x = 0;
	}else{
		sizeCountries = size;
	}

	HTable temp = malloc(sizeof(hashtable));
	temp->info = malloc(sizeof(pbucket ) * size);
	for(int i = 0 ; i < size ; i++){
		temp[i]= NULL;
	}
	
	return temp;

}


//Adjusted hash function found in K08 DOMES DEDOMENWN
int hash(char* key , int M){

	unsigned int h=0;
	int i;

	for(i=0 ; i< strlen(key) ; i++){
		h=(h<<5 | h>>27);
		h+=(unsigned int)key[i];
	}

	return h % M;

}

//Inserts a node to the hash tables
void hash_insert(HTable disease ,HTable countries , LNode temp){

	int hd = hash(temp->record->diseaseID , sizeDisease);
	int hc = hash(temp->record->country , sizeCountries);

	//if table is empty in hd create bucket
	if(disease->info[hd] == NULL){
		disease->info[hd] = malloc(sizeof(bucket));
		disease->info[hd]->counter = 0;
		disease->info[hd]->link = NULL;
		disease->info[hd]->item = malloc(sizeof(Bitem)*itemArraySize);
		disease->info[hd]->item[0] = malloc(sizeof(bucketItem));
		disease->info[hd]->item[0]->array = malloc(strlen(temp->record->diseaseID)+1);
		strcpy(disease->info[hd]->item[0]->array , temp->record->diseaseID);
		disease->info[hd]->item[0]->root = NULL;
		disease->info[hd]->item[0]->root = BTinsert(disease->info[hd]->item[0]->root , temp);	
	}
	else if(disease->info[hd] != NULL){

		int flag=1;
		for(int i = 0 ; i<=disease->info[hd]->counter ; i++){																//Search bucket until same value found or counter is reached
			if(!strcmp(disease->info[hd]->item[i]->array , temp->record->diseaseID)){
				disease->info[hd]->item[i]->root = BTinsert(disease->info[hd]->item[i]->root , temp);
				flag = 0;
				break;
			}
		}
		if(flag){
			if(disease->info[hd]->counter < itemArraySize-1){																//if bucket has space add it in the bucket and increace counter
				disease->info[hd]->counter++;
				disease->info[hd]->item[disease->info[hd]->counter] = malloc(sizeof(bucketItem));
				disease->info[hd]->item[disease->info[hd]->counter]->array = malloc(strlen(temp->record->diseaseID)+1);
				strcpy(disease->info[hd]->item[disease->info[hd]->counter]->array , temp->record->diseaseID);
				disease->info[hd]->item[disease->info[hd]->counter]->root = NULL;
				disease->info[hd]->item[disease->info[hd]->counter]->root = BTinsert(disease->info[hd]->item[disease->info[hd]->counter]->root , temp);
				flag = 0;
			}
			else if(disease->info[hd]->link == NULL){																		//else if bucket full and link is NULL create new bucket
				disease->info[hd]->link = malloc(sizeof(bucket));
				disease->info[hd]->link->counter = 0;
				disease->info[hd]->link->link = NULL;
				disease->info[hd]->link->item = malloc(sizeof(Bitem)*itemArraySize);
				disease->info[hd]->link->item[0] = malloc(sizeof(bucketItem));
				disease->info[hd]->link->item[0]->array = malloc(strlen(temp->record->diseaseID)+1);
				strcpy(disease->info[hd]->link->item[0]->array , temp->record->diseaseID);
				disease->info[hd]->link->item[0]->root = NULL;
				disease->info[hd]->link->item[0]->root = BTinsert(disease->info[hd]->link->item[0]->root , temp);	
				flag = 0;			
			}else{

				pbucket traversal = disease->info[hd]->link;
				pbucket current;
				
				while(traversal!=NULL){																						//else search each available bucket until you find same value
					for(int i = 0 ; i<=traversal->counter ; i++){
						if(!strcmp(traversal->item[i]->array , temp->record->diseaseID)){
							traversal->item[i]->root = BTinsert(traversal->item[i]->root , temp);
							flag = 0;
							break;
						}
					}
					if(!flag) break;

					current = traversal;
					traversal = traversal->link;
				}

				if(flag){																							//if failed to find same value and bucket not full insert it in current bucket
						if(current->counter < itemArraySize-1){
							current->counter++;
							current->item[current->counter] = malloc(sizeof(bucketItem));
							current->item[current->counter]->array = malloc(strlen(temp->record->diseaseID)+1);
							strcpy(current->item[current->counter]->array , temp->record->diseaseID);
							current->item[current->counter]->root = NULL;
							current->link->item[current->counter]->root = BTinsert(current->item[current->counter]->root , temp);
						}
						else{																						//else create new bucket and insert it there
							current->link = malloc(sizeof(bucket));
							current->link->counter = 0;
							current->link->link = NULL;
							current->link->item = malloc(sizeof(Bitem)*itemArraySize);
							current->link->item[0] = malloc(sizeof(bucketItem));
							current->link->item[0]->array = malloc(strlen(temp->record->diseaseID)+1);
							strcpy(current->link->item[0]->array , temp->record->diseaseID);
							current->link->item[0]->root = NULL;
							current->link->item[0]->root = BTinsert(current->link->item[0]->root , temp);	
						}
				}
			}
		}
	}

	//Same process as above
	if(countries->info[hc] == NULL){
		countries->info[hc] = malloc(sizeof(bucket));
		countries->info[hc]->counter = 0;
		countries->info[hc]->link = NULL;
		countries->info[hc]->item = malloc(sizeof(Bitem)*itemArraySize);
		countries->info[hc]->item[0] = malloc(sizeof(bucketItem));
		countries->info[hc]->item[0]->array = malloc(strlen(temp->record->country)+1);
		strcpy(countries->info[hc]->item[0]->array , temp->record->country);
		countries->info[hc]->item[0]->root = NULL;
		countries->info[hc]->item[0]->root = BTinsert(countries->info[hc]->item[0]->root , temp);

	}
	else if(countries->info[hc] != NULL){

		int flag = 1;
		for(int i = 0 ; i<=countries->info[hc]->counter ; i++){
			if(!strcmp(countries->info[hc]->item[i]->array , temp->record->country)){
				countries->info[hc]->item[i]->root = BTinsert(countries->info[hc]->item[i]->root , temp);
				flag = 0;
				break;
			}
		}
		if(flag){

			if(countries->info[hc]->counter < itemArraySize-1){
				countries->info[hc]->counter++;
				countries->info[hc]->item[countries->info[hc]->counter] = malloc(sizeof(bucketItem));
				countries->info[hc]->item[countries->info[hc]->counter]->array = malloc(strlen(temp->record->country)+1);
				strcpy(countries->info[hc]->item[countries->info[hc]->counter]->array , temp->record->country);
				countries->info[hc]->item[countries->info[hc]->counter]->root = NULL;
				countries->info[hc]->item[countries->info[hc]->counter]->root = BTinsert(countries->info[hc]->item[countries->info[hc]->counter]->root , temp);
				flag = 0;
			}
			else if(countries->info[hc]->link == NULL){
				countries->info[hc]->link = malloc(sizeof(bucket));
				countries->info[hc]->link->counter = 0;
				countries->info[hc]->link->link = NULL;
				countries->info[hc]->link->item = malloc(sizeof(Bitem)*itemArraySize);
				countries->info[hc]->link->item[0] = malloc(sizeof(bucketItem));
				countries->info[hc]->link->item[0]->array = malloc(strlen(temp->record->country)+1);
				strcpy(countries->info[hc]->link->item[0]->array , temp->record->country);
				countries->info[hc]->link->item[0]->root = NULL;
				countries->info[hc]->link->item[0]->root = BTinsert(countries->info[hc]->link->item[0]->root , temp);
				flag = 0;	
			}else{

				pbucket traversal = countries->info[hc]->link;
				pbucket current;
				
				while(traversal!=NULL){

					for(int i = 0 ; i<=traversal->counter ; i++){
						if(!strcmp(traversal->item[i]->array , temp->record->country)){
							traversal->item[i]->root = BTinsert(traversal->item[i]->root , temp);
							flag = 0;
							break;
						}
					}
					if(!flag) break;

					current = traversal;
					traversal = traversal->link;
					
				}

				if(flag){
						if(current->counter < itemArraySize-1){
							current->counter++;
							current->item[current->counter] = malloc(sizeof(bucketItem));
							current->item[current->counter]->array = malloc(strlen(temp->record->country)+1);
							strcpy(current->item[current->counter]->array , temp->record->country);
							current->item[current->counter]->root = NULL;
							current->item[current->counter]->root = BTinsert(current->item[current->counter]->root , temp);
						}
						else{
							current->link = malloc(sizeof(bucket));
							current->link->counter = 0;
							current->link->link = NULL;
							current->link->item = malloc(sizeof(Bitem)*itemArraySize);
							current->link->item[0] = malloc(sizeof(bucketItem));
							current->link->item[0]->array = malloc(strlen(temp->record->country)+1);
							strcpy(current->link->item[0]->array , temp->record->country);
							current->link->item[0]->root = NULL;
							current->link->item[0]->root = BTinsert(current->link->item[0]->root , temp);
					}
				}
			}
		}
	}

}


//Find people that are still infected by target
int hash_infected(HTable disease , char *target , int M){

	int h = hash(target, M);
	int size = 0;

	if(disease->info[h]==NULL) return 0;

	for(int i=0 ; i <= disease->info[h]->counter ; i++){
		if(!strcmp(disease->info[h]->item[i]->array , target)){
			size = BTinfected(disease->info[h]->item[i]->root);
			return size;
		}
	}

	if(disease->info[h]->link != NULL){
		
		pbucket temp = disease->info[h]->link;
		
		while(temp!=NULL){
			for(int i=0 ; i <= temp->counter ; i++){
				if(!strcmp(temp->item[i]->array , target)){
					size = BTinfected(temp->item[i]->root);
					return size;
				}
			}

			temp = temp->link;
		}
	}

	return size;
}

//Free hash table
void hash_destroy(HTable table , int total){

	if(table == NULL) return;

    for(int i = 0 ; i < total ; i++){
        if(table->info[i]!=NULL){
            for(int j = 0 ; j <= table->info[i]->counter ; j++){
                free(table->info[i]->item[j]->array);
                BTdestroy(table->info[i]->item[j]->root);
                free(table->info[i]->item[j]);
            }

            if(table->info[i]->link!=NULL){
              	destroyBucket(table->info[i]->link);
            }

            free(table->info[i]->item);
            free(table->info[i]);

        }
    }
    free(table->info);
    free(table);
}

//free bucket
void destroyBucket(pbucket current){

	if(current->link != NULL){
		destroyBucket(current->link);
	}

	for(int i = 0 ; i <= current->counter ; i++){
		free(current->item[i]->array);
		BTdestroy(current->item[i]->root);
		free(current->item[i]);
	}

	free(current->item);
	free(current);
}
