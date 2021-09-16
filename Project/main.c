#include "header.h"

int main(void){
	clock_t begin = clock();
	
	reading_dataset_x();

	clock_t end = clock();
	printf("Program run time :  %f\n", (double)(end - begin) / CLOCKS_PER_SEC);

	return 0;
}
