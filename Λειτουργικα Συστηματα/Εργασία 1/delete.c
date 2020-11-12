#include <stdio.h>
#include "prompt.h"

//Implementation of delete
void delete(HTable records, YNode *head, pcode *start, int ID){

	//if not found return else continue
	if(hashDelete(records, head, start, ID)) return;

	printf("Record %d deleted\n", ID);
}