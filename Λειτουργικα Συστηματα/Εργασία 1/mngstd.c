#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "prompt.h"

int main(int argc , char *argv[]){
	if(argc == 2 || argc == 4){
		printf("Need more argumets!\n");
		command_guide();
	}
	if(argc > 5){
		printf("Too many arguments!\n");
		command_guide();
	}

	int flag1=0;
	int flag2=0;

	FILE *fp_inp = NULL;
	FILE *fp_conf = NULL;

	for(int i=1 ; i < argc ; i+=2){
		if(!strcmp(argv[i],"-i")){
			if(flag1){
				printf("Error! Second time -i argument was seen.\n");
				command_guide();
			}

			if(i+1==argc){
				printf("Error! -i argument is at the end! No file name after it.\n");
				command_guide();
			}

			if(!strcmp(argv[i+1] , "-c")){
				printf("Error! Command line arguments are not passed properly.\n");
				printf("Expected file after -i\n");
				command_guide();
			}
			
			fp_inp = fopen(argv[i+1] , "r+");
			
			if(fp_inp == NULL){
				printf("Error opening file! Please check if file name after -i is correct. Exiting...\n");
				exit(0);
			} 

			flag1 = 1;

		}else if(!strcmp(argv[i],"-c")){
			if(flag2){
				printf("Error! Second time -c argument was seen.\n");
				command_guide();
			}

			if(i+1==argc){
				printf("Error! -c argument is at the end! Cannot find disease hash table size after it.\n");
				command_guide();
			}

			if(!strcmp(argv[i+1] , "-i")){
				printf("Error! Command line arguments are not passed properly.\n");
				printf("Expected file after -c\n");
				command_guide();
			}

			fp_conf = fopen(argv[i+1] , "r+");
			
			if(fp_conf == NULL){
				printf("Error opening file! Please check if file name after -c is correct. Exiting...\n");
				exit(0);
			}

			flag2 = 1;
		}else{
			printf("Command line arguments are not passed properly.\n");
			printf("Expected -i or -c before filename\n");
			command_guide();
		}
	}

	prompt(fp_inp, fp_conf);
	
	return 0;
}
