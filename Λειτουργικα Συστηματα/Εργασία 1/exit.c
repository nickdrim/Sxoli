#include <stdio.h>
#include "prompt.h"

//Implementation of exit
void programmExit(YNode head, pcode start, HTable records){
	
	//Function calls to free structures
	postFreeList(start);
	yearFreeList(head);
	hashFree(records);

	printf("exit program\n");
}