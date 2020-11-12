#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"
#include "records.h"
#include "binary_tree.h"
#include "linked_list.h"

int size_of_disease;
int size_of_countries;

void command_guide(){
	printf("\nThis is a guide to help you pass arguments to the program properly!\n\n");

	printf("Program needs exactly 9 arguments in order to work\n\n");

	printf("Format should look like this :\n\n");

	printf("./diseaseMonitor -p patientRecordsFile –h1 diseaseHashtableNumOfEntries –h2 countryHashtableNumOfEntries –b bucketSize\n\n");

	printf("Where :\n");
	printf("./diseaseMonitor is the name of the programm\n");
	printf("-p is used to help programm understand that after this argument is the file name\n");
	printf("patientRecordsFile is the name of the file\n");
	printf("-h1 is used to help programm understand that after this argument is an integer regarding the size of disease hash table\n");
	printf("diseaseHashtableNumOfEntries is an integer that determines the size of the hashtable of diseases\n");
	printf("-h2 is used to help programm understand that after this argument is an integer regarding the size of countries hash table\n");
	printf("countryHashtableNumOfEntries is an integer that determines the size of the hashtable of countries\n");
	printf("-b is used to help programm understand that after this argument is an integer regarding the size of the bucket of the hash table\n");
	printf("bucketSize is an integer that determines the size of the bucket of the hash tables in bytes\n");

	printf("\nOrder of -p -h1 -h2 -b can differ as long as after them is the matching discription\n");
	printf("For example:\n\n");

	printf("./diseaseMonitor –h1 diseaseHashtableNumOfEntries –b bucketSize -p patientRecordsFile –h2 countryHashtableNumOfEntries \n\n");

	printf("Is also correct\n");

	printf("\nExiting...\n");

	exit(0);
}

void menu(FILE *fp , int diseaseSize , int countrySize , int BucketSize){
	
	HTable disease = hash_initialize(diseaseSize , BucketSize);
	HTable countries = hash_initialize(countrySize , BucketSize);

	

	LNode last = NULL;
	LNode head = NULL;
	initialize(fp , disease , countries , &head , &last);

	//printList(head);

	if(head == NULL){
	//	printf("Error! Initializing list failed. File might be empty");
		printf("error\n");
		programmExit(head , disease , countries);
	}

	size_of_disease = diseaseSize;
	size_of_countries = countrySize;
	int x = 1;
	int flag = 1;
	
	do{
	//	printf("Please enter a command...\n");
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
	    	arr = realloc(arr, sizeof(char *) * counter);
	    }

	    token = strtok(line, " ");
	    if(counter == 0) token = strtok(token , "\n");
	    for(int i=0 ; i < counter ; i++){
	        arr[i]=malloc(strlen(token)+1);
	        strcpy(arr[i] , token);
	        //printf("arr[%d] = %s\n", i , arr[i]);
	        if(i==counter-1) arr[i] = strtok(arr[i],"\n");
	        token = strtok(NULL , " ");
	        if(token == NULL) break;
	    }

	    if(!strcmp(arr[0] , "/globalDiseaseStats")){
	    	if(counter > 1){
	    		if(counter > 3){
	    		//	printf("Too many arguments. Try again\n");
	    			printf("error\n");
	    		}else if(counter == 2){
	    			//printf("Too few arguments. Try again.\n");
	    			printf("error\n");
	    		}else{
	    			globalDiseaseStats(disease , arr[1] , arr[2] , NULL);
	    		}
	    	}else{
	    		globalDiseaseStats(disease , NULL);
	    	}
	    }else if(!strcmp(arr[0], "/diseaseFrequency")){

	    	if(counter < 4) printf("error\n");//printf("Error! Too few arguments!\n");
	    	else if(counter > 5) printf("error\n");//printf("Error! Too many arguments!\n");		
	    	else{
	    		if(!validDate(arr[2])){
	    			//printf("Error! Entry date is not correct format\n");
	    			printf("error\n");
	    		}else if(!validDate(arr[3])){
	    			//printf("Error! Exit date is not correct format\n");
	    			printf("error\n");
	    		}else{
	    			if(counter == 4){
	    				diseaseFrequency(disease , arr[1] , arr[2] , arr[3] , NULL);
	    			}else if(isString(arr[4])){
	    				//printf("Country name must be a string with no numbers\n");
	    				printf("error\n");
	    			}else{
	    				diseaseFrequency(disease , arr[1] , arr[2] , arr[3] , arr[4]);
	    			}
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "/topk-Diseases")){

	    	if(counter < 3) printf("error\n");//printf("Error! Too few argumets!\n");
	    	else if(counter == 4) printf("error\n");//printf("Error! If entry date is given an exit date must be given aswell\n");
	    	else if(counter > 5)printf("error\n");// printf("Error! Too many arguments\n");
	    	else if(counter == 3){
	    		if(!validInt(arr[1])){
	    			if(!isString(arr[2])){
	    				topk_Diseases(disease , atoi(arr[1]) , arr[2] , NULL , NULL);
	    			}
	    			else{
	    			//	printf("Third argument must be a string with no numbers! Your input : %s\n", arr[2]);
	    				printf("error\n");
	    			}
	    		}
	    		else{
	    		//	printf("Second argument must be a number! Your input : %s\n", arr[1]);
	    			printf("error\n");
	    		}
	    	}else{
	    		if(!validInt(arr[1])){
	    			if(!isString(arr[2])){
	    				if(validDate(arr[3])){
	    					if(validDate(arr[4])){
	    						topk_Diseases(disease , atoi(arr[1]) , arr[2] , arr[3] , arr[4]);
	    					}else{
	    						//printf("Fourth argument must be a date in DD-MM-YYYY format\n");
	    						printf("error\n");
	    					}
	    				}else{
	    					//printf("Fourth argument must be a date in DD-MM-YYYY format\n");
	    					printf("error\n");
	    				}
	    			}else{
	    				//printf("Third argument must be a string with no numbers! Your input : %s\n", arr[2]);
	    				printf("error\n");
	    			}
	    		}else{
	    		//	printf("Second argument must be a number! Your input : %s\n", arr[1]);
	    			printf("error\n");
	    		}
	    	}
	    }else if(!strcmp(arr[0] , "/topk-Countries")){

	    	if(counter < 3) printf("error\n");//printf("Error! Too few argumets!\n");
	    	else if(counter == 4) printf("error\n");//printf("Error! If entry date is given an exit date must be given aswell\n");
	    	else if(counter > 5) printf("error\n");//printf("Error! Too many arguments\n");
	    	else if(counter == 3){
	    		if(!validInt(arr[1])){
					topk_Countries(countries , atoi(arr[1]) , arr[2] , NULL , NULL);
	    		}
	    		else{
	    			//printf("Second argument must be a number! Your input : %s\n", arr[1]);
	    			printf("error\n");
	    		}
	    	}else{
	    		if(!validInt(arr[1])){
    				if(validDate(arr[3])){
    					if(validDate(arr[4])){

    						topk_Countries(countries , atoi(arr[1]) , arr[2] , arr[3] , arr[4]);

    					}else{
    						//printf("Fourth argument must be a date in DD-MM-YYYY format\n");
    						printf("error\n");
    					}
    				}else{
    				//	printf("Fourth argument must be a date in DD-MM-YYYY format\n");
    					printf("error\n");
    				}
	    		}else{
	    		//	printf("Second argument must be a number! Your input : %s\n", arr[1]);
	    			printf("error\n");
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "/insertPatientRecord")){
	    	
	    	if(counter < 7)printf("error\n");// printf("Error! Too few arguments!\n");
	    	else if(counter > 8) printf("error\n");// printf("Error! Too many arguments!\n");
	    	else{
	    		if(validInt(arr[1])){
	    		//	printf("Error! Second argument must be an integer ID. Your input %s\n" , arr[1]);
	    			printf("error\n");
	    		}else if(isString(arr[2])){
	    	//		printf("Error! Third argument must be a string with no numbers! Your input %s\n" , arr[2]);
	    			printf("error\n");
	    		}else if(isString(arr[3])){
	    	//		printf("Error! Fourth argument must be a string with no numbers! Your input %s\n" , arr[3]);
	    			printf("error\n");
	    		}else if(isString(arr[5])){
	    	//		printf("Error! Sixth argument must be a string with no numbers! Your input %s\n" , arr[3]);
	    			printf("error\n");
	    		}
	    		else if(!validDate(arr[6])){
	    		//	printf("Error! Entry date is not correct format\n");
	    		}
	    		else{

	    			LNode temp = newNode(arr , counter);

	    			last = list_insert(last , temp);
	    			hash_insert(disease , countries , temp);

	    		//	printf("Record has been successfully added!\n");
	    			printf("Record added\n");
	    		}
	    	}


	    }else if(!strcmp(arr[0] , "/recordPatientExit")){
	    	if(counter > 3){
	    		//printf("Error! Too many arguments.\n");
	    		printf("error\n");
	    	}else if(counter < 3){
	    		//printf("Error! Too few arguments\n");
	    		printf("error\n");
	    	}
	    	else{
	    		if(validInt(arr[1])){
	    		//	printf("Error! Second argument must be an integer!\n");
	    			printf("error\n");
	    		}else if(!validDate(arr[2])){
	    	//		printf("Error! Date is not correct format\n");
	    			printf("error\n");
	    		}else{
	    			recordPatientExit(head , atoi(arr[1]) , arr[2]);
	    		}
	    	}

	    }else if(!strcmp(arr[0] , "/numCurrentPatients")){

	    	if(counter > 1){
	    		if(counter > 2){
	    		//	printf("Too many arguments\n");
	    			printf("error\n");
	    		}else{
	    			numCurrentPatients(disease , arr[1]);
	    		}
	    	}else{
	    		numCurrentPatients(disease , NULL);
	    	}


	    }else if(!strcmp(arr[0] , "/exit")){
	    	if(counter > 1) printf("error\n");//printf("Error! If you want to exit type /exit with no words after it\n");
	    	else{
		    	
		    	programmExit(head , disease , countries);
		    	for(int i=0 ; i < counter ; i++){
			    	free(arr[i]);
			    }
		    	free(arr);
		    	free(copy);
		    	free(line);
		    	fclose(fp);
		    	printf("exiting\n");
		    	exit(0);
		    	
			}
	    }else{
	    //	printf("asdasd\n");
	    	//printf("Invalid input\n");
	    	//commands();
	    	//printf("error\n");
	    	break;
	    }

	    for(int i=0 ; i < counter ; i++){
			free(arr[i]);
		}
		free(arr);
    	free(copy);
    	free(line);

	}while(x);

}

//Initializes hash tables and list with records
void initialize(FILE *fp , HTable disease , HTable countries , LNode *head , LNode *last){

    char *word = NULL;
    size_t len = 0;
    LNode temp = NULL;
    char *token;
    int counter;

    while(getline(&word , &len , fp)!=-1){
    	counter = 0;
    	token = strtok(word , " ");

    	temp = malloc(sizeof(nodes));
	    temp->record = malloc(sizeof(records));
		temp->next = NULL;  
    	
    	while( token != NULL ) {
    		counter++;
	    	switch(counter){
	    		case 1:
	    			//check if word is a valid integer and not a string
        			if(!validInt(token)){
						//printf("Error! ID of a record of the file containts letter! Exiting...\n");
						programmExit(*head , disease , countries);
						free(temp->record);
	   					free(temp);
						printf("error\n");
						exit(0);
					}

					//Check if id already exists
					if(*head!=NULL){
						if(list_search(head , atoi(token))){
							//printf("Error! Another record of the file has the same id! Exiting...\n");
							programmExit(*head , disease , countries);
							free(temp->record);
	   						free(temp);
							printf("error\n");
							exit(0);
						}
					}

					temp->record->recordID = atoi(token);
	    			break;
	    		case 2:
	    			//check if first name contains number
        			if(isString(token)){
        			//	printf("Error! First name of a record contains a number! Exiting...\n");
        				programmExit(*head , disease , countries);
        				free(temp->record);
	   					free(temp);
        				printf("error\n");
						exit(0);
        			}

        			temp->record->patientFirstName = malloc(strlen(token) + 1);
        			strcpy(temp->record->patientFirstName , token);
	    			break;
	    		case 3:
	    			//check if last name contains number
        			if(isString(token)){
        				//printf("Error! Last name of a record contains a number! Exiting...\n");
        				programmExit(*head , disease , countries);
        				free(temp->record);
	   					free(temp);
        				printf("error\n");
						exit(0);
        			}

        			temp->record->patientLastName = malloc(strlen(token) + 1);
        			strcpy(temp->record->patientLastName , token);
	    			break;	
	    		case 4:
	    			temp->record->diseaseID = malloc(strlen(token) + 1);
					strcpy(temp->record->diseaseID , token);
					break;
	    		case 5:
	    			if(isString(token)){
        				//printf("Error!Country of a record contains a number! Exiting...\n");
        				printf("error\n");
        				programmExit(*head , disease , countries);
        				free(temp->record);
	   					free(temp);
						exit(0);
        			}
					temp->record->country = malloc(strlen(token) + 1);
					strcpy(temp->record->country , token);
	    			break;	
	    		case 6:
	    			if(!validDate(token)){
	    				printf("error\n");
						programmExit(*head , disease , countries);
						free(temp->record);
	   					free(temp);
						exit(0);
					}

					temp->record->entryDate = malloc(strlen(token) + 1);
					strcpy(temp->record->entryDate , token);
	    			break;
	    		case 7:
	    			strtok(token , "\n");
	    			if(!validDate(token) && !strcmp(token , "-")){
	    				printf("error\n");
						programmExit(*head , disease , countries);
						free(temp->record);
	   					free(temp);
						exit(0);
					}

					temp->record->exitDate = malloc(strlen(token) + 1);
					strcpy(temp->record->exitDate , token);
	    			break;
	    	}
	    
	    	token = strtok(NULL, " ");
	   	}

	   	if(counter != 7 ){
	   	//	printf("Error! Cannot complete record. File is not properly written\n");
	   		printf("error\n");
	   		programmExit(*head , disease , countries);
	   		*last =NULL;
	   		free(temp->record);
	   		free(temp);
	   		exit(0);
	   	}else{
	   		if(*last==NULL && *head == NULL){
	        	*head = *last = temp;
	        	hash_insert(disease , countries , temp);
	    	}else{
	    		LNode copy;
	    		copy = *last;
	        	*last = list_insert(copy , temp);
	        	hash_insert(disease , countries , temp);
	    	}
	    	temp = NULL;
	   	}
    }
    free(word);

}

////////////////////////////// PROGRAMM FUNCTIONS /////////////////////////////////////////////

int diseaseFrequency(HTable disease , char *virusName , char* start , char* end , char* place){

	int size = 0;
	int h = hash(virusName , size_of_disease);
	if(place == NULL){
		if(disease->info[h]!=NULL){
			for(int j=0 ; j <= disease->info[h]->counter ; j++){
				if(!strcmp(disease->info[h]->item[j]->array , virusName)){
					size = getSizeDates(disease->info[h]->item[j]->root , start , end);
					printf("%s %d\n", disease->info[h]->item[j]->array , size);
					//printf("Total instances of %s around the world between %s and %s is : %d\n", virusName , start , end , size);
					return size;
				}
			}
			if(disease->info[h]->link!=NULL){
				pbucket next = disease->info[h]->link;
				while(next != NULL){
					for(int j=0 ; j <= next->counter ; j++){
						if(!strcmp(next->item[j]->array , virusName)){
							size = getSizeDates(next->item[j]->root , start , end);
							printf("%s %d\n", next->item[j]->array , size);
							//printf("Total instances of %s around the world between %s and %s is : %d\n", virusName , start , end , size);	
							return size;								
						}
					}

					next = next->link;
				}
			}			
		}

		printf("%s %d\n", virusName , size);

	}else{
		if(disease->info[h]!=NULL){
			for(int j=0 ; j <= disease->info[h]->counter ; j++){
				if(!strcmp(disease->info[h]->item[j]->array , virusName)){
					size = getSizeCountry(disease->info[h]->item[j]->root , start , end , place);
					printf("%s %d\n", disease->info[h]->item[j]->array , size);
					//printf("Total instances of %s in %s between %s and %s is : %d\n", virusName , place , start , end , size);
					return size;
				}
			}
			if(disease->info[h]->link!=NULL){
				pbucket next = disease->info[h]->link;
				while(next != NULL){
					for(int j=0 ; j <= next->counter ; j++){
						if(!strcmp(next->item[j]->array , virusName)){
							size = getSizeCountry(next->item[j]->root , start , end , place);
							printf("%s %d\n", next->item[j]->array , size);
							//printf("Total instances of %s in %s between %s and %s is : %d\n", virusName , place , start , end , size);	
							return size;
						}
					}
					next = next->link;
				}
			}			
		}
		//printf("No results found for %s\n", virusName);
	}


	return size;
}

void numCurrentPatients(HTable disease , char* target){

	int size = 0;

	if(target == NULL){
		if(disease != NULL){
			for(int i=0 ; i < size_of_disease ; i++){
				if(disease->info[i]!=NULL){
						for(int j=0 ; j <= disease->info[i]->counter ; j++){
							size = BTinfected(disease->info[i]->item[j]->root);
							printf("%s %d\n", disease->info[i]->item[j]->array , size);
							//printf("Total instances of %s around the world is : %d\n",disease->info[i]->item[j]->array , size);
						}
						if(disease->info[i]->link!=NULL){
							pbucket next = disease->info[i]->link;
							while(next != NULL){
								for(int j=0 ; j <= next->counter ; j++){
									size = BTinfected(next->item[j]->root);
									printf("%s %d\n", next->item[j]->array , size);
									//printf("Total instances of %s around the world is : %d\n", next->item[j]->array , size);
								}
								next=next->link;
							}
						}
					}
			}
			return;
		}
	}else{
		size = hash_infected(disease , target , size_of_disease);
		//printf("Total people still infected by %s is : %d\n" , target , size);
		printf("%s %d\n", target , size);
		return;
	}
}

/////////////////////////// Variadic Function /////////////////////////////////
void globalDiseaseStats(HTable disease , const char *str,...){

	va_list ap;

	va_start (ap, str);
	const char *target=str;
	int size=0;
	int x = 1;

	if(target==NULL){
		va_end (ap);
		for(int i=0 ; i < size_of_disease ; i++){
			if(disease->info[i]!=NULL){
				for(int j=0 ; j <= disease->info[i]->counter ; j++){
					size = getSize(disease->info[i]->item[j]->root);
					printf("%s %d\n", disease->info[i]->item[j]->array , size);
					//printf("Total instances of %s around the world is : %d\n",disease->info[i]->item[j]->array , size);
				}
				if(disease->info[i]->link!=NULL){
					pbucket next = disease->info[i]->link;
					while(next != NULL){
						for(int j=0 ; j <= next->counter ; j++){
							size = getSize(next->item[j]->root);
							printf("%s %d\n", next->item[j]->array , size);
							//printf("Total instances of %s around the world is : %d\n", next->item[j]->array , size);
						}
						next=next->link;
					}
				}
			}
		}
	}else{
		
		char *start = malloc(strlen(target)+1);
		strcpy(start,target);
		target = va_arg (ap,const char *);
		char *end = malloc(strlen(target)+1);
		strcpy(end,target);
		va_end (ap);
		strtok(end , "\n");

		if(!validDate(start) || !strcmp(start , "-")){
			//printf("Error! Start date is wrong format\n");
			printf("error\n");
			return;
		}
		if(!validDate(end) || !strcmp(end , "-")){
			//printf("Error! End date is wrong format\n");
			printf("error\n");
			return;
		}
		if(compareDate(start , end) == 1){
			//printf("Error! Entry date cannot be smaller than exit date.\n");
			printf("error\n");
			return;
		}


		for(int i=0 ; i < size_of_disease ; i++){
			if(disease->info[i]!=NULL){
				for(int j=0 ; j <= disease->info[i]->counter ; j++){
					size = getSizeDates(disease->info[i]->item[j]->root , start , end);
					printf("%s %d\n", disease->info[i]->item[j]->array, size);
					//printf("Total instances of %s around the world between %s and %s is : %d\n",disease->info[i]->item[j]->array , start , end , size);
				}
				if(disease->info[i]->link!=NULL){
					pbucket next = disease->info[i]->link;
					while(next != NULL){
						for(int j=0 ; j <= next->counter ; j++){
							size = getSizeDates(next->item[j]->root ,start , end);
							printf("%s %d\n", next->item[j]->array , size);
							//printf("Total instances of %s around the world between %s and %s is : %d\n",next->item[j]->array , start , end , size);
						}
						next = next->link;
					}
				}
			}
		}

		free(start);
		free(end);
	}
   
}

void recordPatientExit(LNode head , int id , char* end){

	LNode temp = list_search(&head , id);

	if(temp == NULL){
		//printf("Patient with id = %d was not found\n",id);
		printf("Not found\n");
		return;
	}

	//printf("Previous release date = %s\n", temp->record->exitDate);					//uncomment to see change

	if(!strcmp(temp->record->exitDate , "-")){
		free(temp->record->exitDate);
		temp->record->exitDate = malloc(strlen(end)+1);
		strcpy(temp->record->exitDate , end);
		//printf("Patient with id = %d has just been released. Release date : %s\n", id , end);
		printf("Record updated\n");
	}
	else if(compareDate(end , temp->record->entryDate) == 1){
		free(temp->record->exitDate);
		temp->record->exitDate = malloc(strlen(end)+1);
		strcpy(temp->record->exitDate , end);
		//printf("Patient with id = %d has just been released. Release date : %s\n", id , end);
		printf("Record updated\n");
	}
	else{
		//printf("Cannot change exit date! Exit date is smaller than entry date\n");
		printf("error\n");
	}

}

void topk_Diseases(HTable disease , int k , char *place , char *start , char *end ){

	int size = 0;
	QHead q = createQueue();								//queue is needed to make tree complete before heap
	HPTree heap = NULL;
	int total = 0;


	for(int i = 0 ; i < size_of_disease ; i++){
		if(disease->info[i]!=NULL){
			for(int j=0 ; j <= disease->info[i]->counter ; j++){
				size = 0;
				size = getSizeCountry(disease->info[i]->item[j]->root , start , end , place);
				if(size!=0){
					total++;
					heap = heapInsert(heap , size , disease->info[i]->item[j]->array , q);
				}
			}
			if(disease->info[i]->link!=NULL){
				pbucket next = disease->info[i]->link;
				while(next != NULL){
					for(int j=0 ; j <= next->counter ; j++){
						size = 0;
						size = getSizeCountry(next->item[j]->root , start , end , place);
						if(size!=0){
							total++;
							heap = heapInsert(heap , size , next->item[j]->array , q);
						}
					}
					next = next ->link;
				}
			}
		}
	}


	if(heap == NULL){
		//printf("No diseases in %s are recorded\n" , place);
		printf("error\n");
		return;
	}

	if(k > total){
		//if(start == NULL) printf("There are less than %d diseases in %s. Printing results for top %d diseases\n", k , place , total);
		//else printf("There are less than %d diseases in %s between %s and %s. Printing results for top %d diseases\n", k , place , start , end , total);
		k = total;
	}

	while(q->front != NULL){
		deQueue(q);
	}

	free(q);

	HPTree temp = NULL;

	//make Heap 

	for(int i=total/2 ; i > 0 ; i--){
		heap = heapify(heap , i , &temp);
	}

	//if still not a heap make heap from last swap
	if(temp!=NULL && !isHeap(heap)){
		int x = heapSize(temp);
		HPTree temp2 = NULL;
		while(x > 1){
			for(int i=x/2 ; i > 0 ; i--){
				temp = heapify(temp , i , &temp2);
			}
			x = heapSize(temp2);
			temp = temp2;
			temp2 = NULL;
		}

	}

	int j = total;

	//if(start == NULL) printf("Printing top %d diseases in %s : \n", k , place);
	//else printf("Printing top %d diseases in %s between %s and %s : \n", k , place , start , end);

	for(int i=0 ; i < k ; i++){
		heap = heapMax(heap , j , i+1);
		j--;
	}

	if(size > 0){
		heapDestroy(heap);
	//	printf("Heap destroyed\n");
	}
}

void topk_Countries(HTable countries , int k , char *virusName , char *start , char *end ){

	int size = 0;
	QHead q = createQueue();
	HPTree heap = NULL;
	int total = 0;

	for(int i = 0 ; i < size_of_countries ; i++){
		if(countries->info[i]!=NULL){
			for(int j=0 ; j <= countries->info[i]->counter ; j++){
				size = 0;
				size = getSizeDisease(countries->info[i]->item[j]->root , start , end , virusName);
				if(size!=0){
					total++;
					heap = heapInsert(heap , size , countries->info[i]->item[j]->array , q);
				}
			}
			if(countries->info[i]->link!=NULL){
				
				pbucket next = countries->info[i]->link;
				
				while(next != NULL){
					for(int j=0 ; j <= next->counter ; j++){
						size = 0;
						size = getSizeDisease(next->item[j]->root , start , end , virusName);
						if(size!=0){
							total++;
							heap = heapInsert(heap , size , next->item[j]->array , q);
						}
					}
					next = next->link;
				}
			}
		}
	}

	if(heap == NULL){
	//	printf("No countries with %s are recorded\n" , virusName);
		printf("error\n");
		return;
	}

	if(k > total){
	//	if(start == NULL) printf("There are less than %d countries with %s. Printing results for top %d countries\n", k , virusName , total);
	//	else printf("There are less than %d countries with %s between %s and %s. Printing results for top %d countries\n", k , virusName , start , end , total);
		k = total;
	}

	while(q->front != NULL){
		deQueue(q);
	}

	free(q);

	HPTree temp = NULL;

	for(int i=total/2 ; i > 0 ; i--){
		heap = heapify(heap , i , &temp);
	}

	if(temp!=NULL && !isHeap(heap)){
		int x = heapSize(temp);
		HPTree temp2 = NULL;
		while(x > 1){
			for(int i=x/2 ; i > 0 ; i--){
				temp = heapify(temp , i , &temp2);
			}
			x = heapSize(temp2);
			temp = temp2;
			temp2 = NULL;
		}

	}

	int j = total;

	//if(start == NULL) printf("Printing top %d countries with %s : \n", k , virusName);
	//else printf("Printing top %d countries with %s between %s and %s : \n", k , virusName , start , end);

	
	for(int i=0 ; i < k ; i++){
		heap = heapMax(heap , j , i+1);
		j--;
	}

	if(size > 0){
		heapDestroy(heap);
	//	printf("Heap destroyed\n");
	}
}

void programmExit(LNode head , HTable disease , HTable countries){

	hash_destroy(disease , size_of_disease);
	hash_destroy(countries , size_of_countries);

	list_destroy(head);

//	printf("All structs have been successfully destroyed!\n");

	return;
}

///////////////////////////// HELPING FUNCTIONS ///////////////////////////////

int validInt(char *word){
	char test[50];
	sprintf(test , "%d" , atoi(word));

	return strcmp(test,word);										//if string is same after making it an integer then argument is int
}

//Checks if string is in proper date format
int validDate(char *word){

	if(strlen(word) == 1){
		if(strcmp(word , "-")){
			//printf("Error! Exit date of a record is invalid!\n");
			printf("error\n");
			return 0;
		}

		return 1;
	}
	if(strlen(word)!=10){
		//printf("Error! Entry date of a record is invalid!\n");
		printf("error\n");
		return 0;
	}
	if(word[0] - '0' < 0 || word[0] - '0' > 3){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}
	if(word[0] - '0' == 3 && word[1] - '0' > 1){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}
	if(word[1] - '0' < 0 || word[4] - '0' < 0 || word[7] - '0' < 0 || word[8] - '0' < 0 || word[9] - '0' < 0){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}
	if(word[0] - '0' == 0 && word[1] - '0' == 0){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}
	if(word[2] != '-' || word[5] != '-'){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}
	if(word[3] - '0' != 0 && word[3] - '0' != 1){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}
	if(word[3] - '0' == 0 && word[4] - '0' == 0){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}
	if(word[6] - '0' < 1 || word[6] - '0' > 2){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		printf("error\n");
		return 0;
	}

	return 1;
}

//Checks if argument is string
int isString(char *word){
	int check;
    return check = atoi(word);					// it check = 0 then it is string
}

int compareDate(char *rootString , char *newString){   
    
	int len1 = strlen(rootString);
	int len2 = strlen(newString);


    char x[len1];
    strcpy(x , rootString);
    char y[len2];
    strcpy(y , newString);
    char *ptrx;
    char *ptry;
    int i[3];
    int j[3];

    
    char *token1 = strtok(x , "-");
    
    int k=0;

    while(token1!=NULL){
        i[k] = strtol(token1 , &ptrx , 10);
        k++;
        token1 = strtok(NULL , "-");
    }
    
    char *token2 = strtok(y , "-");
    k=0;
    
    while(token2!=NULL){
        j[k] = strtol(token2 , &ptry , 10);
        k++;
        token2 = strtok(NULL , "-");
    }
    // if i > j rootString > newString (1) if i = j rootString = newString (0) if i < j rootString < newString (-1)
    if(i[2] < j[2]){
        return -1;
    }else if(i[2] > j[2]){
        return 1;
    }else{
        if(i[1] < j[1]){
            return -1;
        }else if(i[1]>j[1]){
            return 1;
        }else{
            if(i[0] < j[0]){
                return -1;
            }else if(i[0]>j[0]){
                return 1;
            }
            else return 0;
        }
    }
}