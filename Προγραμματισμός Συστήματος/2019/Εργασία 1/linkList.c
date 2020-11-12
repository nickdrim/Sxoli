#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"
#include "help.h"


//Prints containts of list. Nowhere in programm or in comment
void printList(LNode head){

	if(head == NULL) return;

	LNode temp = head;

	while(temp!=NULL){
		printf("%d\n", temp->record->recordID );
		temp= temp->next;
	}
}

//Searches if id is already in list
LNode list_search(LNode *head , int x){
	if((*head)->next == NULL){
		if((*head)->record->recordID == x){
			return *head;
		}
	}else{

		LNode traversal = (*head)->next;

		while(traversal != NULL){
			if(traversal->record->recordID == x){
				return traversal;
			}

			traversal = traversal->next;
		}
	}

	return NULL;
}

//Inserts node to the list from tail
LNode list_insert(LNode tail , LNode node){

	tail->next = node;
	tail = node;

	return tail;
}

//Makes a new node used in insertPatientRecord
LNode newNode(char** arr , int counter){

	LNode temp = malloc(sizeof(nodes));
	temp->record = malloc(sizeof(records));
	temp->next = NULL;
	
	temp->record->recordID = atoi(arr[1]);
	
	temp->record->patientFirstName = malloc(strlen(arr[2]) + 1);
	strcpy(temp->record->patientFirstName , arr[2]);

	temp->record->patientLastName = malloc(strlen(arr[3]) + 1);
	strcpy(temp->record->patientLastName , arr[3]);

	temp->record->diseaseID = malloc(strlen(arr[4]) + 1);
	strcpy(temp->record->diseaseID , arr[4]);

	temp->record->country = malloc(strlen(arr[5]) + 1);
	strcpy(temp->record->country , arr[5]);

	temp->record->entryDate = malloc(strlen(arr[6]) + 1);
	strcpy(temp->record->entryDate , arr[6]);
	
	if(counter == 7){
		temp->record->exitDate = malloc(strlen("-") + 1);
		strcpy(temp->record->exitDate , "-");
	}else{
		if(!validDate(arr[7])){
			//printf("Error! Exit date is not correct format\n");
			printf("error\n");
			return NULL;
		}
		temp->record->exitDate = malloc(strlen(arr[7]) + 1);
		strcpy(temp->record->exitDate , arr[7]);
	}

	return temp;
}

//Free list
void list_destroy(LNode head){

	if(head==NULL) return;

	LNode current = head;
	LNode traversal = head->next;

	free(current->record->patientFirstName);
	free(current->record->patientLastName);
	free(current->record->diseaseID);
	free(current->record->country);
	free(current->record->entryDate);
	free(current->record->exitDate);
	free(current->record);
	free(current);

	while(traversal != NULL){
		current = traversal;
		traversal=traversal->next;
		free(current->record->patientFirstName);
		free(current->record->patientLastName);
		free(current->record->diseaseID);
		free(current->record->country);
		free(current->record->entryDate);
		free(current->record->exitDate);
		free(current->record);
		free(current);
	}

	return;

}