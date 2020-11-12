#include <stdio.h>
#include <stdlib.h>
#include "prompt.h"

//Create a bucket node and return a pointer to it
pbucket newBucketNode(SNode data){
	pbucket temp = malloc(sizeof(bucket));
	temp->inverted = data;
	temp->chain = NULL; 

	return temp;
}

//Searches the list to find the ID
SNode bucketListSearch(pbucket head, int ID){
	if(listEmpty(head)) return 0;

	if(head->inverted->info->StudentID == ID){
		return head->inverted;
	}else{
		pbucket temp = head->chain;

		while(temp != NULL){
			if(temp->inverted->info->StudentID == ID){
				//printf("Student Found\n");
				return temp->inverted;
			}
		
			temp = temp->chain;
		}
	}

	return NULL;
}

//Inserts a node to the list
void bucketListInsert(pbucket *head, SNode data){		
		pbucket temp = newBucketNode(data);

		temp->chain = *head;
		*head = temp;
}

//Removes all the memory allocated for the list
void bucketFreeList(pbucket head){
	if(head == NULL) return;

	pbucket temp;

	while(head != NULL){
		temp = head;
		head = head->chain;
		temp->inverted = NULL;
		free(temp);
	}

}

//Deletes a node from the list
void bucketListDelete(pbucket *head, SNode data){
	// return if either of them is NULL
	if((*head == NULL) || (data == NULL)) return;

	if((*head)->inverted == data){
		pbucket temp = *head;

		*head = (*head)->chain;
		temp->inverted = NULL;
		temp->chain = NULL;
		free(temp);

	}else{
		pbucket current = (*head)->chain;
		pbucket previous = *head;

		while(current != NULL){
			if(current->inverted == data){

				previous->chain = current->chain;
				current->inverted = NULL;
				current->chain = NULL;
				free(current);
				return;
			}
		
			previous = current;
			current = current->chain;
		}
	}

	return;
}