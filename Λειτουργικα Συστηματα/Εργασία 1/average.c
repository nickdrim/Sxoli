#include <stdio.h>
#include "prompt.h"

//Implementation of average
void average(YNode head, int date){
	if(listEmpty(head)) return;

	//Search for date
	YNode temp = yearListSearch(head,date);

	//if not found or empty
	if((temp == NULL) || (temp->counter == 0)){
		printf("No students enrolled in %d\n",date);
		return;
	}

	pbucket traverse = temp->blist;
	float sum = 0.0;

	//Sum gpas of all students in date
	while(traverse != NULL){
		sum += traverse->inverted->info->gpa;
		traverse = traverse->chain;
	}

	printf("%f\n", sum/temp->counter);

}