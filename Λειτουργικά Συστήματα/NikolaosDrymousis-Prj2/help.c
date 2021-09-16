#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "help.h"


//Checks if string contains numbers
int isString(char *word){
	int check;

	//if check = 0 then it is string
    return check = atoi(word);
}

//Checks if string is a valid number
int validInt(char *word){
	char test[50];
	sprintf(test , "%d" , atoi(word));

	//if string is same after making it an integer then argument is int
	return strcmp(test, word);
}

//Checks if string is a valid float
int validFloat(const char *str, float *number){
    int len;
    float dummy = 0.0;
    if (sscanf(str, "%f %n", &dummy, &len) == 1 && len == (int)strlen(str)){
        if(dummy >= 0.0 && dummy <=10.0){
            *number = dummy;
            return 0;
        }else{
            printf("[%s] is not between 0 and 10 (%.7g).\n", str, dummy);
        }
    }else printf("[%s] is not valid (%.7g)\n", str, dummy);

    return 1;
}

int compareString(char *firstString, char* secondString){
	return strcmp(firstString, secondString);
}

void find_max(double* arr,int size){
	if(arr == NULL) return;
	
	double max = arr[0];

	for(int x=1 ; x < size ; x++){
		if(arr[x] > max){
			max = arr[x];
		}
	}

	printf("Max Time for Workers : %f\n",max);
}

void find_min(double *arr, int size){
	if(arr == NULL) return;

	double min = arr[0];

	for(int x=1 ; x < size ; x++){
		if(arr[x] < min){
			min = arr[x];
		}
	}

	printf("Min Time for Workers : %f\n",min);
}

void time_of_leafs(double *arr, int size){
	for(int x=0 ; x < size ; x++){
		printf("Time for W%d:  %f\n", x, arr[x]);
	}
}

int count_digit(int number) {
   int count = 0;
   while(number != 0) {
      number = number / 10;
      count++;
   }
   return count;
}