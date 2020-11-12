#include <stdio.h>
#include "prompt.h"

void minimum(YNode head, int date){
	if(listEmpty(head)) return;

	YNode temp = yearListSearch(head,date);

	//if not found or empty
	if((temp == NULL) || (temp->counter == 0)){
		printf("No students enrolled in %d\n",date);
		return;
	}

	pbucket traverse = temp->blist;
	SNode minStudent = NULL;
	float minimum = 10.0;

	//Search for student with lowest gpa
	while(traverse != NULL){
		if(traverse->inverted->info->gpa < minimum){
			minimum = traverse->inverted->info->gpa;
		}
		traverse = traverse->chain;
	}

	traverse = temp->blist;

	//if more than one student have the lowest gpa
	while(traverse != NULL){
		if(traverse->inverted->info->gpa == minimum){
			printf("(%d) ", traverse->inverted->info->StudentID);
		}
		traverse = traverse->chain;
	}

	printf("\n");
}