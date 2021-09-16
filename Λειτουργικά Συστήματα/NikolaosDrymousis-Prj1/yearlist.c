#include <stdio.h>
#include <stdlib.h>
#include "prompt.h"

//A function that all lists can use to see if they are empty
int listEmpty(void* head){
	if(head == NULL){
		printf("List is empty\n");
		return 1;
	}

	return 0;
}

//Creates and returns a year node
YNode newYearNode(SNode data){
	YNode temp = malloc(sizeof(ynode));
	temp->year=data->info->entryYear;
	temp->blist = newBucketNode(data);
	temp->counter = 1;
	temp->next = NULL;

	return temp;
}

//Searches the list and return a pointer to the node with date
YNode yearListSearch(YNode head, int date){
	if(listEmpty(head)) return 0;

	if(head->year == date){
		//printf("Student Found\n");
		return head;
	}else{
		
		YNode temp = head->next;

		while(temp != NULL){
			
			if(temp->year == date){
				//printf("Student Found\n");
				return temp;
			}
		
			temp = temp->next;
		}

		//printf("Student not found\n");
		return NULL;
	}
}

//Inserts a new student node to the list
void yearListInsert(YNode *head , SNode data){
	if(*head == NULL){
		*head = newYearNode(data);
	}else{
		if(data->info->entryYear == (*head)->year){
			bucketListInsert(&((*head)->blist), data);
			(*head)->counter++;
		}else{

			YNode traverse = (*head)->next;
			YNode previous = *head;

			if(traverse == NULL){
				YNode temp = newYearNode(data);

				if(data->info->entryYear > (*head)->year){
					temp->next = *head;
					*head = temp;
				}else{
					(*head)->next = temp;
				}

				return;
			}

			//Places the new node to its right chronological order
			while(traverse != NULL){
				if(traverse->year == data->info->entryYear){
					bucketListInsert(&(traverse->blist), data);
					traverse->counter++;
					return;
				}
				else if(traverse->year < data->info->entryYear){
					YNode temp = newYearNode(data);
					temp->next = traverse;
					previous->next = temp;
					return;
				}

				previous = traverse;
				traverse = traverse->next;
			}

			YNode temp = newYearNode(data);
			previous->next = temp;
		}
	}
}

//Searches the node with year and deletes the student node in that yearlist
void yearListDelete(YNode *head, int ID, int year){
	if(*head == NULL) return;

	YNode temp = yearListSearch(*head, year);

	bucketListDelete(&(temp->blist), bucketListSearch(temp->blist, ID));
	temp->counter--;

	return;

}

//Frees the space that was allocated from the list
void yearFreeList(YNode head){

	if(head == NULL) return;

	YNode temp;

	while(head != NULL){
		temp = head;
		head = head->next;
		bucketFreeList(temp->blist);
		temp->next = NULL;
		free(temp);
	}
}

//Sorts the list so that it is in chronological order
void yearListSort(YNode *head){
	if(listEmpty(head)) return;

	YNode temp = *head;

	temp = (*head)->next;
	(*head)->next = temp->next;
	temp->next = *head;
	*head = temp;

	YNode previous = (*head)->next;
	YNode current = previous->next;

	while((current != NULL) && (previous->year < current->year)){

		previous->next = current->next;
		temp->next = current;
		current->next = previous;

		temp = temp->next;
		previous = temp->next;
		current = previous->next;

	}
}