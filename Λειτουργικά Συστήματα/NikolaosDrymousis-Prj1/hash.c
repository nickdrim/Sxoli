#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prompt.h"
#include "help.h"

const float a = 0.6;											//load factor
int size = 0;													//Size of hash table
int defaultSize = 100;											//default value for no input or config file


//The hash function we use
int hash(int x){

	int length = snprintf( NULL, 0, "%d", x );
	char* str = malloc( length + 1 );
	snprintf( str, length + 1, "%d", x );

	unsigned int h=0;
	int i;

	for(i=0 ; i< strlen(str) ; i++){
		h=(h<<5 | h>>27);
		h+=(unsigned int)str[i];
	}

	free(str);

	return h % size;

}

//Initialize all the data structures 
HTable hashInitialize(FILE *fp_input, FILE *fp_config, YNode *head, pcode *start){

	HTable hash = NULL;

	//if there is no config file
	if(fp_config == NULL){
		//if there is an input file
		if(fp_input == NULL){
			
			size = defaultSize / a;

			hash = malloc(sizeof(hashtable) * size);

			for(int i=0 ; i < size ; i++){
				hash[i].start = NULL;
			}

			return hash;
		//if there is an input file
		}else{

			char c;
			int noLines=0;

			//counts the lines of the input file to determine
			//a good size for the hashtable
			while((c=getc(fp_input)) != EOF){
				if(c == '\n')
					noLines++;
			}

			size = noLines / a;

			hash = malloc(sizeof(hashtable) * size);

			for(int i=0 ; i < size ; i++){
				hash[i].start = NULL;
			}

			rewind(fp_input);
		}
	//if there is a config file
	}else{
		char *word = NULL;
    	size_t len = 0;
    	char *token;
    	int counter = 0;
		
		getline(&word, &len, fp_config);
		token = strtok(word, "\n");

		//if the configuration doesnt contain a valid integer greater than 0
		if(validInt(token) || atoi(token) < 1){
			//if there is no inpt file default size is used
			if(fp_input == NULL){
				printf("Config file is invalid.Setting default size for hash table\n");
				size = defaultSize / a;
			//if there is an input file determine a good size
			}else{
				printf("Config file is invalid.Setting hash table size according to input file\n");
				char c;
				int noLines=0;

				while((c=getc(fp_input)) != EOF){
					if(c == '\n')
						noLines++;
				}

				size = noLines / a;

				hash = malloc(sizeof(hashtable) * size);

				for(int i=0 ; i < size ; i++){
					hash[i].start = NULL;
				}

				//so we can use it again
				rewind(fp_input);
			}
		}

		//if there is no size determined yet it means config file is valid
		if(!size){
			size = atoi(token); /// a;

			hash = malloc(sizeof(hashtable) * size);

			for(int i=0 ; i < size ; i++){
				hash[i].start = NULL;
			}
		}

		free(word);
		fclose(fp_config);
	}

	//if there is no input file no need to do the process below
	if(fp_input == NULL) return hash;

	char *word = NULL;
    size_t len = 0;
    pstudent temp = NULL;
    char *token;
    int counter;
    int total = 0;

    //reads input file line by line and splits this line into tokens
    while(getline(&word, &len, fp_input)!=-1){
    	int flag = 0;
    	counter = 0;
    	float score = 0.0;
    	token = strtok(word, " ");

    	temp = newStudent();

    	while( token != NULL){
    		if(flag) break;
    		counter++;
    		switch(counter){
	    		case 1:
	    			//check if word is a valid integer and not a string
        			if(validInt(token)){
						//printf("Error! ID of a record of the file containts letter! Exiting...\n");
						programmExit(*head,*start, hash);
	   					free(temp);
	   					free(word);
	   					fclose(fp_input);
						printf("error\n");
						exit(0);
					}

					//Check if id already exists
					if(hashSearch(hash, atoi(token)) != NULL){
						//printf("Student %d exists\n", atoi(token));
						total ++;
	   					free(temp);
	   					flag = 1;
	   					break;
					}

					temp->StudentID = atoi(token);
	    			break;
	    		case 2:
	    			//check if first name contains number
        			if(isString(token)){
        				printf("Error! First name of a record contains a number! Exiting...\n");
        				programmExit(*head, *start ,hash);
	   					free(temp);
	   					free(word);
        				fclose(fp_input);
        				printf("error\n");
						exit(0);
        			}
        			temp->firstName = malloc(strlen(token) + 1);
        			strcpy(temp->firstName, token);
	    			break;
	    		case 3:
	    			//check if last name contains number
        			if(isString(token)){
        				printf("Error! Last name of a record contains a number! Exiting...\n");
        				programmExit(*head, *start, hash);
        				free(temp->firstName);
	   					free(temp);
	   					free(word);
	   					fclose(fp_input);
        				printf("error\n");
						exit(0);
        			}

        			temp->lastName = malloc(strlen(token) + 1);
        			strcpy(temp->lastName , token);
	    			break;	
	    		case 4:

	    			if(validInt(token)){
						//printf("Error! ID of a record of the file containts letter! Exiting...\n");
						programmExit(*head, *start, hash);
						free(temp->firstName);
						free(temp->lastName);
	   					free(temp);
	   					free(word);
	   					fclose(fp_input);
						printf("error\n");
						exit(0);
					}

	    			temp->zip = atoi(token);
					break;
	    		case 5:
	    			if(validInt(token)){
						//printf("Error! ID of a record of the file containts letter! Exiting...\n");
						programmExit(*head, *start , hash);
						free(temp->firstName);
						free(temp->lastName);
	   					free(temp);
	   					free(word);
	   					fclose(fp_input);
						printf("error\n");
						exit(0);
					}
					
					temp->entryYear = atoi(token);
	    			break;	
	    		case 6:

	    			if(validFloat(token, &score)){
	    				printf("error\n");
						programmExit(*head, *start, hash);
						free(temp->firstName);
						free(temp->lastName);
	   					free(temp);
	   					free(word);
	   					fclose(fp_input);
						exit(0);
					}

					temp->gpa = score;			
	    			break;
		    }

		    token = strtok(NULL, " ");
    	}



    	if(counter != 6 && !flag){
	   	//	printf("Error! Cannot complete record. File is not properly written\n");
	   		printf("error\n");
	   		programmExit(*head, *start, hash);
	   		free(temp->firstName);
			free(temp->lastName);
			free(temp);
			fclose(fp_input);
	   		exit(0);
	   	}else if(!flag){

	   		//if all good call insert functions
	   		SNode data = newStudentNode(temp);

	   		postListInsert(start, data);
	        yearListInsert(head, data);
	        hashInsert(hash, data);

	   	}
	}

	printf("%d dupblicated records found and not inserted\n", total);

	free(word);
	fclose(fp_input);

	yearListSort(head);

	return hash;
}

//Inserts a record to the hash table
void hashInsert(HTable records, SNode temp){

	int h = hash(temp->info->StudentID);

	if(records[h].start == NULL){
		records[h].start = temp;
	}else{
		studentListInsert(&(records[h].start), temp);
	}
}

//Searches for a value in the hash table
SNode hashSearch(HTable records, int ID){

	if(records == NULL) return NULL;

	int h = hash(ID);

	if(records[h].start == NULL){
		return NULL;
	}else{
		return studentListSearch(records[h].start, ID);
	}
}

//Deletes a value from the hash table and all other structures that must be affected
int hashDelete(HTable records, YNode *head, pcode *start, int ID){
	
	int h = hash(ID);

	SNode temp = hashSearch(records, ID);

	if(temp == NULL){
		printf("Student %d does not exists \n", ID);
		return 1;
	}

	//call delete of all other data structures
	yearListDelete(head, ID, temp->info->entryYear);
	postListDelete(start, ID, temp->info->zip);
	studentListDelete(&(records[h].start), ID);
	return 0;
}

//Releases the memory that was allocated for the hash table data structure
void hashFree(HTable records){
	if(records == NULL) return;

	for(int i=0 ; i < size ; i++){
		if(records[i].start != NULL){
			studentFreeList(records[i].start);
		}
	}

	free(records);
}