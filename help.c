#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "help.h"


//Instructions
void command_guide(void){
	printf("\nThis is a guide to help you pass arguments to the program properly!\n\n");

	printf("Program needs at least 3 arguments or no more or less than 5 in order to work\n\n");

	printf("Format should look like this :\n\n");

	printf("./mngstd -i inputfile –c configfile\n\n");
	printf("-c configfile is not necessary");

	printf("Where :\n");
	printf("./mngstd is the name of the programm\n");
	printf("-i is used to help programm understand that after this argument is the input file name\n");
	printf("inputfile is the name of the file that contains the data\n");
	printf("-c is used to help programm understand that after this argument is the configuration file name\n");
	printf("configfile is the name of the file that contains configurations\n");

	printf("\nOrder of -i and -c can differ as long as after them is the matching discription\n");
	printf("For example:\n\n");

	printf("./mngstd –c configfile –i inputfilen\n\n");

	printf("Is also correct\n");

	printf("\nExiting...\n");

	exit(0);
}

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