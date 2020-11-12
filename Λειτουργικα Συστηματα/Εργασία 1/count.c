#include <stdio.h>
#include "prompt.h"

//Implementation of counta
void count(YNode head){
	//if there is no list or there is only one node but is empty
	if(listEmpty(head) || ((head->counter == 0) && (head->next == NULL))){
		printf("No students are enrolled\n");
		return;
	}

	YNode temp = head;

	//Print all non empty nodes
	while(temp!=NULL){
		//if empty ignore
		if(temp->counter == 0){
			temp=temp->next;
			continue;
		}
		printf("({%d , %d}) ", temp->year, temp->counter);
		temp=temp->next;
	}
	printf("\n");
}