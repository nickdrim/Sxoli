#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "help.h"

int main(int argc , char *argv[]){
	if(argc<9){
		//printf("Need more argumets!\n");
		printf("error\n");
		command_guide();
		exit(0);
	}
	if(argc>9){
		//printf("Too many arguments!\n");
		printf("error\n");
		command_guide();
		exit(0);
	}

	int flag1=0;
	int flag2=0;
	int flag3=0;
	int flag4=0;

	FILE *fp;
	int disHashSize=0;
	int	couHashSize=0;
	int bucketSize=0;

	for(int i=1 ; i<argc ; i++){
		if(!strcmp(argv[i],"-p")){
			if(flag1){
				//printf("Error! Second time -p argument was seen.\n");
				//command_guide();
				printf("error\n");
			}
			if(i+1==argc){
				//printf("Error! -p argument is at the end! No file name after it.\n");
				//command_guide();
				printf("error\n");
			}
			if(!strcmp(argv[i+1] , "-h1") || !strcmp(argv[i+1] , "-h2") || !strcmp(argv[i+1] , "-b")){
				//printf("Command line arguments are not passed properly.\n");
				//command_guide();
				printf("error\n");
			}
			
			fp = fopen(argv[i+1] , "r+");
			
			if(fp==NULL){
				//printf("Error opening file! Please check if file name after -p is correct. Exiting...\n");
				printf("error\n");
				exit(0);
			} 

			flag1 = 1;

		}else if(!strcmp(argv[i],"-h1")){
			if(flag2){
				//printf("Error! Second time -h1 argument was seen.\n");
				//command_guide();
				printf("error\n");
			}

			if(i+1==argc){
				//printf("Error! -h1 argument is at the end! Cannot find disease hash table size after it.\n");
				//command_guide();
				printf("error\n");
			}

			if(!strcmp(argv[i+1] , "-p") || !strcmp(argv[i+1] , "-h2") || !strcmp(argv[i+1] , "-b")){
				//printf("Command line arguments are not passed properly.\n");
				//command_guide();
				printf("error\n");
			}
			
			char *pos = strstr(argv[i+1] , ".txt");

			if(pos){
				//printf("Error! File name after -h1.\n");
				//command_guide();
				printf("error\n");
			}

			if(validInt(argv[i+1])){
				//printf("Error! Expected integer after -h1\n");
				//printf("User input: %s\n" , argv[i+1]);
				//command_guide();
				printf("error\n");
			}

			disHashSize = atoi(argv[i+1]);
			flag2 = 1;

		}else if(!strcmp(argv[i],"-h2")){
			if(flag3){
				//printf("Error! Second time -h2 argument was seen.\n");
				//command_guide();
				printf("error\n");
			}

			if(i+1==argc){
				//printf("Error! -h2 argument is at the end! Cannot find country hash table size after it.\n");
				//command_guide();
				printf("error\n");
			}

			if(!strcmp(argv[i+1] , "-p") || !strcmp(argv[i+1] , "-h1") || !strcmp(argv[i+1] , "-b")){
				//printf("Command line arguments are not passed properly.\n");
				//command_guide();
				printf("error\n");
			}

			char *pos = strstr(argv[i+1] , ".txt");
			
			if(pos){
				//printf("Error! File name after -h2.\n");
				//command_guide();
				printf("error\n");
			}

			if(validInt(argv[i+1])){
				//printf("Error! Expected integer after -h2\n");
				//printf("User input: %s\n" , argv[i+1]);
				//command_guide();
				printf("error\n");
			}

			couHashSize = atoi(argv[i+1]);
			flag3 = 1;

		}else if(!strcmp(argv[i],"-b")){
			if(flag4){
				//printf("Error! Second time -b argument was seen.\n");
				//command_guide();
				printf("error\n");
			}

			if(i+1==argc){
				//printf("Error! -b argument is at the end! Cannot find bucket size after it.\n");
				//command_guide();
				printf("error\n");
			}

			if(!strcmp(argv[i+1] , "-p") || !strcmp(argv[i+1] , "-h2") || !strcmp(argv[i+1] , "-h1")){
				//printf("Command line arguments are not passed properly.\n");
				//command_guide();
				printf("error\n");
			}

			char *pos = strstr(argv[i+1] , ".txt");

			if(pos){
				//printf("Error! File name after -b.\n");
				//command_guide();
				printf("error\n");
			}

			if(validInt(argv[i+1])){
				//printf("Error! Expected integer after -b\n");
				//printf("User input: %s\n" , argv[i+1]);
				//command_guide();
				printf("error\n");
			}

			bucketSize = atoi(argv[i+1]);
			flag4 = 1;
		}
	}

	if(bucketSize <  sizeof(bucket)){
		//printf("Error! Bucket size is too small.Please enter a value greater than 16.Exiting...\n");
		printf("error\n");
		fclose(fp);
		exit(0);
	}

	if(disHashSize<=0){
		//printf("Error! Size of hash table for diseases is too small. Please enter a value greater than 1.Exiting...\n");
		printf("error\n");
		fclose(fp);
		exit(0);
	}

	if(couHashSize<=0){
		//printf("Error! Size of hash table for countries is too small. Please enter a value greater than 1.Exiting...\n");
		printf("error\n");
		fclose(fp);
		exit(0);
	}

	//printf("Information :\ndisease hash size = %d\ncountry hash size = %d\nbucket size = %d\n\n",disHashSize , couHashSize , bucketSize);

	menu(fp , disHashSize , couHashSize , bucketSize);
	
	return 0;
}
