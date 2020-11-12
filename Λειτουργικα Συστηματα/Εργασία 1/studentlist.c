#include <stdio.h>
#include <stdlib.h>
#include "prompt.h"

//Searches a student with id
SNode studentListSearch(SNode head, int ID){
	if(listEmpty(head)){
		return NULL;
	}

	if(head->info->StudentID == ID){
		//printf("Student Found\n");
		return head;
	}else{
		
		SNode temp = head->link;

		while(temp != NULL){
			
			if(temp->info->StudentID == ID){
				//printf("Student Found\n");
				return temp;
			}
		
			temp = temp->link;
		}


		//printf("Student not found\n");
		return NULL;
	}
}

//Creates and returns a new student node
SNode newStudentNode(pstudent data){
	SNode temp = malloc(sizeof(snode));

	temp->info = data;
	temp->link = NULL;

	return temp;
}

//Inserts a new student node to the list
void studentListInsert(SNode *head , SNode data){
	if(*head == NULL){
		*head = data;
	}else{

		data->link = *head;
		*head = data;
	}

	return;
}

//Frees all space allocated by the list
void studentFreeList(SNode head){
	if(head == NULL) return;

	SNode temp;

	while(head!=NULL){
		temp = head;
		head = head->link;
		deleteStudent(temp->info);
		free(temp);
	}
}

//Deletes a node in the list
void studentListDelete(SNode *head, int ID){
	if(*head == NULL) return;

	//if node to delete is head change head of the list to next node
	if((*head)->info->StudentID == ID){
		SNode temp = *head;
		*head = (*head)->link;

		temp->link = NULL;
		deleteStudent(temp->info);
		free(temp);
		return;
	}

	SNode current = (*head)->link;
	SNode previous = *head;

	//else traverse the list until node is found or not and if found link the neighbour nodes
	while(current != NULL){
		if(current->info->StudentID == ID){
			previous->link = current->link;
			
			current->link = NULL;
			deleteStudent(current->info);
			free(current);
			return;
		}

		previous = current;
		current = current->link;
	}
}