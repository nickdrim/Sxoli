#include <stdio.h>
#include "prompt.h"

//Implementation of lookup
void lookup(HTable records, int ID){
	//Search record with ID
	SNode temp = hashSearch(records, ID);

	//if found
	if(temp != NULL){
		printf("%d %s %s %d %d %f\n", temp->info->StudentID,
			temp->info->lastName, temp->info->firstName, temp->info->zip, temp->info->entryYear, temp->info->gpa);
	}else{
		printf("Student %d does not exist\n", ID);
	}
}