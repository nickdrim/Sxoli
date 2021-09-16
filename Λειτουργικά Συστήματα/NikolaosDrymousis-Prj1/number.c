#include <stdio.h>
#include "prompt.h"

//Implementation of number
void number(YNode head, int date){
	if(listEmpty(head)) return;

	//Search to find the year we want
	YNode temp = yearListSearch(head,date);

	//if year doesnt exist or does not contain any records
	if((temp == NULL) || (temp->counter == 0)) printf("No students enrolled in %d\n", date);

	printf("%d student(s) in %d\n", temp->counter, date);
}