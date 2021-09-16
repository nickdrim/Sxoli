#include "header.h"

int bsize = 0;

//Creates a logfile for a process with pid = id
void logger(int id, char** countries, int size, int requests,int accepted, int denied){
  FILE *fp = NULL;
  char str[100];
  sprintf(str, "%d", id);

  char* filename = malloc(strlen("log_file.") + strlen(str) + 1);
  strcpy(filename,"log_file.");
  strcat(filename,str);

  if((fp = fopen(filename,"w+")) == NULL){
    perror("LOG_FILE");
    return;
  }

  for(int i = 0 ; i < size-1 ; i++){
    fprintf(fp, "%s\n",countries[i]);
  }

  fprintf(fp, "TOTAL TRAVEL REQUESTS %d\n",requests);
  fprintf(fp, "ACCEPTED %d\n",accepted);
  fprintf(fp, "REJECTED %d\n",denied);

  free(filename);
  fclose(fp);
  printf("Log file produced\n");
}

//Find citizen id and if found write the vaccination info of that citizen
void citizenStatus(HTable records, CHTable citizens,char* citizenID, int fpipe){
  if(records == NULL){
    write(fpipe, "ID NOT FOUND\n", strlen("ID NOT FOUND\n"));
    return;
  }

  check_info data = citizen_hash_return_info(citizens, citizenID);

  if(data == NULL){
    write(fpipe, "ID NOT FOUND\n", strlen("ID NOT FOUND\n"));
    return;
  }else{
    char buf[100];
    sprintf(buf, "%d", data->age);

    write(fpipe, citizenID, strlen(citizenID));
    write(fpipe, " ", strlen(" "));
    write(fpipe, data->firstName, strlen(data->firstName));
    write(fpipe, " ", strlen(" "));
    write(fpipe, data->lastName, strlen(data->lastName));
    write(fpipe, " ", strlen(" "));
    write(fpipe, data->country, strlen(data->country));
    write(fpipe, "\n ", strlen("\n"));
    write(fpipe, "AGE ", strlen("AGE "));
    write(fpipe, buf, strlen(buf));
    write(fpipe, "\n ", strlen("\n"));
  }

  //Else check for all viruses
  for(int i = 0 ; i < get_hashtable_size() ; i++){
    if(records[i].start != NULL){
      
      HNode traverse = records[i].start;

      while(traverse){
        if(bloom_filter_check(traverse->vaccinated, citizenID)){
          LNode temp = skip_lists_search(traverse->vaccinated_levels, traverse->vaccinated_height, atoi(citizenID));
          if(temp){
            write(fpipe, temp->data->virusName, strlen(temp->data->virusName));
            write(fpipe, " ", strlen(" "));
            write(fpipe, temp->data->vaccine, strlen(temp->data->vaccine));
            write(fpipe, " ", strlen(" "));
            write(fpipe, temp->data->date, strlen(temp->data->date));
            write(fpipe, "\n ", strlen("\n"));
          }else{
            write(fpipe, traverse->disease, strlen(traverse->disease));
            write(fpipe, " NO\n", strlen(" NO\n"));
          }
        }else{
          write(fpipe, traverse->disease, strlen(traverse->disease));
          write(fpipe, " NO\n", strlen(" NO\n"));
        }

        traverse = traverse->link;
      }

    }

  }

}

//Check if requested citizen with citizenID is vaccinated and write answer to fifo file
//Also returns the answer to monitor to record the request answer to its info
char* vaccineStatus(HTable records, char* citizenID, char* virusName, char* arrival_date, int write_fd){
  //If hash table is empty no records
  if(records == NULL){
    int num = strlen("NO");
    write(write_fd, &num, sizeof(int));
    write(write_fd, "NO", strlen("NO"));
    return "NO";
  }

  int num = -1;


  int h = hash(virusName, get_hashtable_size());

  //If hashed index is NULL virus does not exist
  if(records[h].start == NULL){
    num = strlen("NO");
    write(write_fd, &num, sizeof(int));
    write(write_fd, "NO", strlen("NO"));
    return "NO";
  }else{
    HNode traverse = records[h].start;

    while(traverse){
      if(!strcmp(traverse->disease, virusName)){
        //Check Bloom filter to see if citizen is possibly vaccinated
        if(bloom_filter_check(traverse->vaccinated, citizenID)){
          //if vaccine status bloom was called from vaccine status
          //search if citizen is truly vaccinated or not      
          LNode temp = skip_lists_search(traverse->vaccinated_levels, traverse->vaccinated_height, atoi(citizenID));

          if(temp){
            num = strlen("YES") + strlen(" ") + strlen(temp->data->date);
            write(write_fd, &num, sizeof(int));
            write(write_fd, "YES", strlen("YES"));
            write(write_fd, " ", strlen(" "));
            write(write_fd, temp->data->date, strlen(temp->data->date));
            if(six_months_prior(temp->data->date, arrival_date)){
              return "YES";
            }else{
              return "NO";
            }
          }else{
            temp = list_node_search(traverse->vaccinated_list, atoi(citizenID));
            if(temp){
              num = strlen("YES") + strlen(" ") + strlen(temp->data->date);
              write(write_fd, &num, sizeof(int));
              write(write_fd, "YES", strlen("YES"));
              write(write_fd, " ", strlen(" "));
              printf("DATE2 %s\n",temp->data->date);
              write(write_fd, temp->data->date, strlen(temp->data->date));
              if(six_months_prior(temp->data->date, arrival_date)){
                return "YES";
              }else{
                return "NO";
              }
            }else{
              num = strlen("NO");
              write(write_fd, &num, sizeof(int));
              write(write_fd, "NO", strlen("NO"));
              return "NO";
            }
          }
        }else{
          num = strlen("NO");
          write(write_fd, &num, sizeof(int));
          write(write_fd, "NO", strlen("NO"));
          return "NO";
        }

      }

      traverse = traverse->link;
    }

    num = strlen("NO");
    write(write_fd, &num, sizeof(int));
    write(write_fd, "NO", strlen("NO"));
    return "NO";
  }
}

//hash function
int hash(char* key, int table_size){
  unsigned int h=0;
	int i;

	for(i=0 ; i < strlen(key) ; i++){
		h=(h<<5 | h>>27);
		h+=(unsigned int)key[i];
	}

	return h % table_size;
}

//Checks if word containts only characters
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
			//printf("error\n");
			return 0;
		}

		return 1;
	}
	if(strlen(word)!=10){
		//printf("Error! Entry date of a record is invalid!\n");
		//printf("error\n");
		return 0;
	}
	if(word[0] - '0' < 0 || word[0] - '0' > 3){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		//printf("error\n");
		return 0;
	}
	if(word[0] - '0' == 3 && word[1] - '0' > 0){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		//printf("error\n");
		return 0;
	}
	if(word[1] - '0' < 0 || word[4] - '0' < 0 || word[7] - '0' < 0 || word[8] - '0' < 0 || word[9] - '0' < 0){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		//printf("error\n");
		return 0;
	}
	if(word[0] - '0' == 0 && word[1] - '0' == 0){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
	//	printf("error\n");
		return 0;
	}
	if(word[2] != '-' || word[5] != '-'){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		//printf("error\n");
		return 0;
	}
	if(word[3] - '0' != 0 && word[3] - '0' != 1){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		//printf("error\n");
		return 0;
	}
	if(word[3] - '0' == 0 && word[4] - '0' == 0){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		//printf("error\n");
		return 0;
	}
	if(word[6] - '0' < 1 || word[6] - '0' > 2){
		//printf("Error! Entry date of a record is invalid! Exiting...\n");
		//printf("error\n");
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

  //printf("Field must be yes or no\n");

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

//Compares the dates to see if the vaccination date is 6 months
//prior to travel date or not. They way it works is that it converts the dates
//into days and substracts them.If the result is smaller than 180 days(6 months)
//than returns 1 else 0
int six_months_prior(char* vaccin_date, char* travel_date){
  if(vaccin_date == NULL || travel_date == NULL) return 0;

  if(compareDate(vaccin_date,travel_date) >= 0) return 0;

  int total_vac = 0;
  int total_travel = 0;
  int temp_vac = 0;
  int temp_travel = 0;


  char vac_day[2];
  vac_day[0] = vaccin_date[0];
  vac_day[1] = vaccin_date[1];

  int k = 1;
  temp_vac = 0;
  for(int i = 0 ; i < 2 ; i++){
    int temp = vac_day[i] - '0';
    for(int j = 0 ; j < k ; j++){
      temp = temp * 10;
    }

    k--;
    temp_vac += temp;
  }

  total_vac += temp_vac;
  
  char trav_day[2];
  trav_day[0] = travel_date[0];
  trav_day[1] = travel_date[1];
  
  k = 1;
  temp_travel = 0;
  for(int i = 0 ; i < 4 ; i++){
    int temp =  trav_day[i] - '0';
    for(int j = 0 ; j < k ; j++){
      temp = temp * 10;
    }

    k--;
    temp_travel += temp;
  }

  total_travel += temp_travel;

  char vac_mon[2];
  vac_mon[0] = vaccin_date[3];
  vac_mon[1] = vaccin_date[4];
  
  k = 1;
  temp_vac = 0;
  for(int i = 0 ; i < 2 ; i++){
    int temp = vac_mon[i] - '0';
    for(int j = 0 ; j < k ; j++){
      temp = temp * 10;
    }

    k--;
    temp_vac += temp;
  }
  
  total_vac += temp_vac * 30;

  char trav_mon[2];
  trav_mon[0] = travel_date[3];
  trav_mon[1] = travel_date[4];
  
  k = 1;
  temp_travel = 0;
  for(int i = 0 ; i < 2 ; i++){
    int temp =  trav_mon[i] - '0';
    for(int j = 0 ; j < k ; j++){
      temp = temp * 10;
    }

    k--;
    temp_travel += temp;
  }

  total_travel += temp_travel * 30;

  char vac_year[4];
  vac_year[0] = vaccin_date[6];
  vac_year[1] = vaccin_date[7];
  vac_year[2] = vaccin_date[8];
  vac_year[3] = vaccin_date[9];

  k = 3;
  temp_vac = 0;
  for(int i = 0 ; i < 4 ; i++){
    int temp = vac_year[i] - '0';
    for(int j = 0 ; j < k ; j++){
      temp = temp * 10;
    }

    k--;
    temp_vac += temp;
  }

  total_vac += temp_vac * 365;

  char trav_year[4];
  trav_year[0] = travel_date[6];
  trav_year[1] = travel_date[7];
  trav_year[2] = travel_date[8];
  trav_year[3] = travel_date[9];

  k = 3;
  temp_travel = 0;
  for(int i = 0 ; i < 4 ; i++){
    int temp =  trav_year[i] - '0';
    for(int j = 0 ; j < k ; j++){
      temp = temp * 10;
    }

    k--;
    temp_travel += temp;
  }

  total_travel += temp_travel * 365;

  return (total_travel - total_vac) <= (30 * 6) ? 1 : 0;

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

int bloom_size_get(void){
  return bsize;
}

void set_bloom_size(int size){
  bsize = size;
}