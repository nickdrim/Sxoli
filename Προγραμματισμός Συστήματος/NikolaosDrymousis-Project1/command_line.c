#include "header.h"

void command_line(HTable records , CHTable citizen_records, PHash country_records){
  int flag = 0;

  do{

    printf("\nPlease enter a command\n");

    char* line = NULL;
    size_t len = 0;
    char* copy = NULL;

    //Get user input
    getline(&line, &len, stdin);
    
    //If user input is just newline continue
    if(!strcmp(line , "\n")){
      free(line);
      continue;
    }

    //make a copy of user input
    copy = malloc(strlen(line) + 1);
    strcpy(copy,line);

    char** commands;
    int commands_size = 0;
    char* saveptr;

    //break user input into tokens and add them to commands table
    char* token = strtok_r(copy, " ", &saveptr);
    
    commands_size++;
    commands = malloc(sizeof(char *) * commands_size);
    commands[0] = malloc(strlen(token) + 1);
    strcpy(commands[0],token);

    while(token != NULL){
      token = strtok_r(NULL, " ", &saveptr);
      if(token == NULL) break;

      commands_size++;
      commands = realloc(commands, sizeof(char *) * commands_size);
      commands[commands_size - 1] = malloc(strlen(token) + 1);
      strcpy(commands[commands_size - 1],token);
    }

    strtok(commands[commands_size - 1], "\n");

    //Uncomment to see commands token by token
    /*for(int i = 0 ; i < commands_size ; i++){
      printf("%s\n", commands[i]);
    }*/

    if(!strcmp(commands[0], "/vaccineStatusBloom")){
      //check if number of arguments is corrent
      if(!no_of_arguments(commands_size, 3, 0)){
        //check if given ID is integer
        if(is_integer(commands[1])){
          vaccineStatusBloom(records, commands[1], commands[2], 0);
        }
      }    
    }else if(!strcmp(commands[0], "/vaccineStatus")){
      //Check if number of arguments is correct
      if(!no_of_arguments(commands_size, 2, 3)){
        //check if given id is integer
        if(is_integer(commands[1])){
          if(commands_size == 2){
            //if virus is not given
            vaccineStatus(records, commands[1], NULL);
          }else{
            //if virus is given
            vaccineStatus(records, commands[1], commands[2]);
          }
        }
      }
    }else if(!strcmp(commands[0], "/populationStatus")){
      //Check if number of arguments is correct
      if(!no_of_arguments(commands_size, 4, 5)){
        //if country is given
        if(commands_size == 4){
          //check if dates are valid
          if(is_date(commands[2])){
            if(is_date(commands[3])){
                //check if second date is bigger than the first
                if(compareDate(commands[2],commands[3]) == -1){
                  populationStatus(records, country_records, commands, commands_size);
                }else{
                  printf("First date must be smaller than second date\n");
                }
            }
          }
        }else{
          //if country is given check if it is a string
          if(is_string(commands[2])){
            if(is_date(commands[3])){
              if(is_date(commands[4])){
                if(compareDate(commands[3],commands[4]) == -1){
                  populationStatus(records, country_records, commands, commands_size);
                }else{
                  printf("First date must be smaller than second date\n");
                }
              }
            }
          }
        }
      }
    }else if(!strcmp(commands[0], "/popStatusByAge")){
      //same as population status
      if(!no_of_arguments(commands_size, 4, 5)){
        if(commands_size == 4){
          if(is_date(commands[2])){
            if(is_date(commands[3])){
              if(compareDate(commands[2],commands[3]) == -1){
                popStatusByAge(records, country_records, commands, commands_size);
              }else{
                printf("First date must be smaller than second date\n");
              }
            }
          }
        }else{
          if(is_string(commands[2])){
            if(is_date(commands[3])){
              if(is_date(commands[4])){
                if(compareDate(commands[3],commands[4]) == -1){
                  popStatusByAge(records, country_records, commands, commands_size);
                }else{
                  printf("First date must be smaller than second date\n");
                }
              }
            }
          }
        }
      }
    }else if(!strcmp(commands[0], "/insertCitizenRecord")){
      if(!no_of_arguments(commands_size, 8, 9)){
        if(commands_size == 8){
          //Check validity of given info
          if(is_integer(commands[1]) && is_string(commands[2]) && is_string(commands[3]) && is_string(commands[4])){
            if(is_integer(commands[5]) && is_valid_age(commands[5])){
              if(yes_or_no(commands[7])){
                if(!strcmp(commands[7], "NO")){
                  insertCitizenRecord(records, citizen_records, commands, commands_size, 0);
                }else{
                  //if answer is yes and no vaccination date is given
                  printf("Error! Please add vaccination date\n");
                }
              }
            }
          }
        }else{
          if(is_integer(commands[1]) && is_string(commands[2]) && is_string(commands[3]) && is_string(commands[4])){
            if(is_integer(commands[5]) && is_valid_age(commands[5])){
              if(yes_or_no(commands[7])){
                if(is_date(commands[8])){
                  insertCitizenRecord(records, citizen_records, commands, commands_size, 0);
                }else{
                  printf("%s\n",commands[8]);
                }
              }
            }
          }
        }
        
      }
    }else if(!strcmp(commands[0], "/vaccinateNow")){
      if(!no_of_arguments(commands_size, 7, 0)){
        if(is_string(commands[1]) && is_string(commands[2]) && is_string(commands[3]) && is_string(commands[4])){
          if(is_integer(commands[5]) && is_valid_age(commands[5])){
            vaccinateNow(records, citizen_records, commands);
          }
        }
      }
    }else if(!strcmp(commands[0], "/list-nonVaccinated-Persons")){
      if(!no_of_arguments(commands_size, 2, 0)){
        listNonVaccinatedPersons(records, commands[1]);
      }
      
    }else if(!strcmp(commands[0], "/exit")){
      printf("Exiting program\n");
      flag = 1;
    }else{
      printf("Wrong command\n");
    }
    
    for(int i = 0 ; i < commands_size ; i++){
      free(commands[i]);
    }
    free(commands);
    free(copy);
    free(line);

    //if exit is given break loop
    if(flag) break;
  
  }while(1);

}

/*Checks if number of arguments given is correct.
Expected is the lowest number of arguments a command can have
Extra is the maximum number of arguments a command can have
Current is the actual number of arguments given*/
int no_of_arguments(int current, int expected, int extra){
  if(current < expected){
    printf("Too few arguments\n");
    return 1;
  }
  if(!extra){
    if(current > expected){
      printf("Too many arguments\n");
      return 1;
    }
  }else{
    if(current > extra){
      printf("Too many arguments\n");
      return 1;
    }
  }

  return 0;
}

//checks if string contains numbers
int is_string(char *word){
  
  for(int i = 0 ; i < strlen(word) ; i++){
    if(word[i] >= 0 && word[i] <= 9){
      return 0;
    }
  }

  return 1;
}

//Checks if given date is valid
int is_date(char *word){
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
	if(word[0] - '0' == 3 && word[1] - '0' > 0){
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

//checks if number contains characters
int is_integer(char* word){
  for(int i = 0 ; i < strlen(word) ; i++){
    if(!isdigit(word[i])){
      return 0;
    }
  }

	return 1;
}

//Checks if number is between valid age range
int is_valid_age(char* word){
  return (atoi(word) > 0 && atoi(word) <= 120);
}

//checks if the answer is YES or NO
int yes_or_no(char* word){
  if(!strcmp(word, "YES")) return 1; 
  if(!strcmp(word, "NO")) return 1;

  printf("Field must be yes or no\n");

  return 0;
}

//Returns current date
char* create_date(void){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  
  char* temp = malloc(sizeof(char) * 11);

  char buffer[10];

  sprintf(buffer, "%d", tm.tm_mday);

  if(strlen(buffer) == 1){
    temp[0] = '0';
    temp[1] = buffer[0];
  }else{
    temp[0] = buffer[0];
    temp[1] = buffer[1];
  }

  temp[2] = '-';

  sprintf(buffer, "%d", tm.tm_mon + 1);

  if(strlen(buffer) == 1){
    temp[3] = '0';
    temp[4] = buffer[0];
  }else{
    temp[3] = buffer[0];
    temp[4] = buffer[1];
  }

  temp[5] = '-';

  sprintf(buffer, "%d", tm.tm_year + 1900);

  temp[6] = buffer[0];
  temp[7] = buffer[1];
  temp[8] = buffer[2];
  temp[9] = buffer[3];
  temp[10] = '\0';

  return temp;

}

//Compares dates
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