#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prompt.h"

//Implementation of the command prompt
void prompt(FILE *fp_inp , FILE *fp_conf){

	//Initialize data structures
	YNode head = NULL;
	pcode start = NULL;
	HTable records = hashInitialize(fp_inp, fp_conf, &head, &start);

	int x = 1;
	int flag = 1;
	
	do{
		printf("Please enter a command...\n");
		char *line = NULL;
	    size_t len = 0;
	    int counter=0;
	    char *copy;

	    getline(&line, &len, stdin);
	    if(!strcmp(line , "\n")){
	    	continue;
	    }
	    
	    if(flag){
			copy = malloc(strlen(line)+1);
	    }else{
	    	copy = realloc(copy , strlen(line)+1);
	    }

	    strcpy(copy,line);
	    char* token = strtok(copy, " ");
	    
	    while(token!=NULL){
	        token = strtok(NULL , " ");
	        counter++;
	    }

	    char **arr;
	    token = NULL;

	    if(flag){
	   		arr = malloc(sizeof(char *) * counter);
	    }else{
	    	arr = realloc(arr , sizeof(char *) * counter);
	    }

	    token = strtok(line, " ");
	    if(counter == 0) token = strtok(token , "\n");
	    for(int i=0 ; i < counter ; i++){
	        arr[i] = malloc(strlen(token)+1);
	        strcpy(arr[i] , token);
	        //printf("arr[%d] = %s\n", i , arr[i]);
	        if(i==counter-1) arr[i] = strtok(arr[i],"\n");
	        token = strtok(NULL , " ");
	        if(token == NULL) break;
	    }

	    if(!strcmp(arr[0] , "i")){
    		
    		if(counter > 7){
    			printf("Too many arguments. Try again\n");
    		}else if(counter < 7){
    			printf("Too few arguments. Try again.\n");
    		}
    		else{
    			int check = 1;

    			for(int i=1 ; i < counter ; i++){
    				if(arr[i] == NULL){
			    		printf("%d argument must not be NULL! Your input : %s\n", i , arr[i]);
			    		check = 0;
			    	}else if(i==2 || i ==3){
			    		if(isString(arr[i])){
			    			printf("%d argument must be a number! Your input : %s\n",i , arr[i]);
			    			check = 0;
			    		}
			    	}else if(i==6){
			    		float x = atoi(arr[i]);
			    		if(validFloat(arr[i], &x)){
			    			printf("%d argument must be float! Your input : %s\n", i, arr[i]);
			    			check = 0;
			    		}
			    	}else{
			    		if(validInt(arr[i])){
			    			printf("%d argument must be a number! Your input : %s\n", i , arr[i]);
			    			check = 0;
			    		}
			    	}
    			}

    			if(check) insert(records, &head, &start, arr);
    		}

	    }else if(!strcmp(arr[0], "l")){

	    	if(counter < 2) printf("Error! Too few arguments!\n");
	    	else if(counter > 2) printf("Error! Too many arguments!\n");	
	    	else if(arr[1] == NULL || validInt(arr[1])){
	    		printf("Second argument must be a number! Your input : %s\n", arr[1]);
	    	}	
	    	else lookup(records, atoi(arr[1]));

	    }else if(!strcmp(arr[0] , "d")){

	    	if(counter < 2) printf("Error! Too few argumets!\n");
	    	else if(counter > 2) printf("Error! Too many arguments\n");
	    	else if(arr[1] == NULL || validInt(arr[1])){
	    		printf("Second argument must be a number! Your input : %s\n", arr[1]);
	    	}
	    	else{delete(records, &head, &start, atoi(arr[1]));}

	    }else if(!strcmp(arr[0] , "n")){

	    	if(counter < 2) printf("Error! Too few argumets!\n");
	    	else if(counter > 2) printf("Error! Too many arguments\n");
	    	else{
	    		if(arr[1] != NULL || !validInt(arr[1])){
					number(head, atoi(arr[1]));
	    		}
	    		else{
	    			printf("Second argument must be a number! Your input : %s\n", arr[1]);
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "t")){
	    	
	    	if(counter < 3) printf("Error! Too few arguments!\n");
	    	else if(counter > 3) printf("Error! Too many arguments!\n");
	    	else{
	    		if(arr[1] == NULL || validInt(arr[1]) || atoi(arr[1]) <= 0){
					printf("Error! Second argument must be a positive integer! Your input %s\n" , arr[1]);
	    		}else if(arr[2] == NULL || validInt(arr[2]) || atoi(arr[1]) <= 0){
					printf("Error! Third argument must be a a positive integer! Your input %s\n" , arr[2]);
	    		}
	    		else{
	    			top(head, atoi(arr[1]), atoi(arr[2]));
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "a")){
	    	if(counter > 2) printf("Error! Too many arguments.\n");
	    	else if(counter < 2) printf("Error! Too few arguments\n");
	    	else{
	    		if(arr[1] == NULL || validInt(arr[1])){
	    			printf("Error! Second argument must be an integer!\n");
	    		}else{
	    			average(head, atoi(arr[1]));
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "m")){

	    	if(counter > 2)	printf("Error! Too many arguments.\n");
	    	if(counter < 2) printf("Error! Too few arguments\n");
	    	else{
	    		if(arr[1] == NULL || validInt(arr[1])){
	    			printf("Error! Second argument must be an integer!\n");
	    		}else{
	    			minimum(head, atoi(arr[1]));
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "c")){
	    	if(counter > 1)	printf("Error! Too many arguments.\n");
	    	else count(head);

	    }else if(!strcmp(arr[0] , "p")){

	    	if(counter > 2)	printf("Error! Too many arguments.\n");
	    	if(counter < 2) printf("Error! Too few arguments\n");
	    	else{
	    		if(arr[1] == NULL || validInt(arr[1])){
	    			printf("Error! Second argument must be an integer!\n");
	    		}else{
	    			postalcode(start, atoi(arr[1]));
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "exit")){
		    	
	    	programmExit(head ,start, records);
	    	for(int i=0 ; i < counter ; i++){
		    	free(arr[i]);
		    }
	    	free(arr);
	    	free(copy);
	    	free(line);
	    	exit(0);
		    	
	    }else{
	    	printf("Invalid input\n");
	    }

	    for(int i=0 ; i < counter ; i++){
			free(arr[i]);
		}
		free(arr);
    	free(copy);
    	free(line);

	}while(x);
}