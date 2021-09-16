#include <stdio.h>
#include <stdlib.h>
#include "prompt.h"

//Implementation of top
void top(YNode head, int number, int date){
	if(listEmpty(head)) return;

	//Search for given date
	YNode temp = yearListSearch(head,date);

	//If not found or empty
	if((temp == NULL) || (temp->counter == 0)){
		printf("No students are enrolled in %d\n", date);
		return;
	}

	pbucket traverse = temp->blist;

	//if asked number of top more than records in that year print all
	if(number >= temp->counter){
		while(traverse != NULL){
			printf("(%d) ",traverse->inverted->info->StudentID);
			traverse = traverse->chain;
		}
		printf("\n");
		return;
	}

	//array with size of number to store best records	
	SNode *array = malloc(sizeof(SNode) * number);

	//Fill array with the first records you find
	for(int i=0 ; i < number ; i++){
		array[i] = traverse->inverted;
		traverse = traverse->chain;
	}

	//cell with least value
	int index = leastValue(array, number);

	//While there are still nodes in that date
	while(traverse != NULL){
		//if a node with less value than the least value of array swap them
		if(array[index]->info->gpa > traverse->inverted->info->gpa){
			array[index]->info->gpa = traverse->inverted->info->gpa;
			index = leastValue(array, number);
		}

		//next node
		traverse = traverse->chain;
	}

	//print top values
	for(int i=0 ; i < number ; i++){
		printf("(%d)  ",array[i]->info->StudentID);
	}
	printf("\n");

	free(array);
}

//Find least value of an array
int leastValue(SNode *arr, int number){
	int index = 0;
	float min = arr[0]->info->gpa;

	for(int i = 1 ; i < number ; i++){
		if(arr[i]->info->gpa < min){
			index = i;
		}
	}

	return index;
}