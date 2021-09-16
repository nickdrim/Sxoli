#include "header.h"

//GLOBAL VARIABLES
//They are global because the might be needed in multiple
//functions and the signal handler in case of signal
//from parent
int waiting = 0;
int received = 0;
//All variables that we need for read and writing to pipes
int read_fifo_pipe = -1;
int write_fifo_pipe = -1;
int fd_size = -1;
int buffer = 0;
int bloom_size = 0;
char* read_file = NULL;
char* mmap_file = NULL;
char* prev_id = NULL;

//Structures we will new
HTable records = NULL;
CHTable citizen_records = NULL;
DHash visited_dir = NULL;

//The idea for them being global was that in case of a terminating signal
//they would be freed by the handler but since we dont need to do that
//i just left them here
char** array = NULL;
char* parser_token = NULL;

//Variables that count all the requests that this process has received
int requests = 0;
int accepted = 0;
int rejected = 0;

//Variables for sigwait
sigset_t set;
int sig;
int *sigptr = &sig;

//If the process starts for the first time it is 1
//Else if previous process was terminated from a SIGINT or SIGQUIT or something else
//that it stays 0 and the newly created process does not have to wait for a signal
//to read fifo files to reinitialize
int first_time = 0;

int main(int argc, char *argv[]){
  srand(time(NULL));

  printf("PROCCESS %d started\n",getpid());

  //open read fifo file
  if((read_fifo_pipe = open(argv[1], O_CREAT | O_RDONLY | O_NONBLOCK, 0777)) == -1){
    printf("process %d %s\t",getpid(),argv[1]);
    perror("fifo open 1:");
    exit(1);
  }

  //open write fifo file
  if((write_fifo_pipe = open(argv[2], O_CREAT | O_WRONLY | O_NONBLOCK, 0777)) == -1){
    printf("process %d %s\t",getpid(),argv[2]);
    perror("fifo open 2:");
    exit(1);
  }

  //Initialize bloom size buffer size and if its the first time procces start
  bloom_size = atoi(argv[3]);
  buffer = atoi(argv[4]);
  first_time = atoi(argv[5]);
  prev_id = argv[6];

  //Initializes the read file the contents of the read fifo file 
  //will be stored in
  initialize_read_file();

  //Initialize structures
  records = hash_initialize(bloom_size);
  citizen_records = citizen_hash_initialize();
  visited_dir = dir_hash_initialize();

  //Initialize sigwait signals
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGQUIT);
  sigaddset(&set, SIGSEGV);

  //Initialize signal handler
  struct sigaction sa = {0};
	sa.sa_handler = signal_handler;

  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGSEGV, &sa, NULL);

  //inform parent that process is ready
  printf("%d initializes\n",getpid());
  kill(getppid(),SIGUSR1);

  //read the fifo file(Line 366 for function)
  read_fifo();

  printf("proccess %d ready\n",getpid());
  //wait for parent procces to read information
  //from all proccesses before continuing 
  //if(first_time) sigwait(&set,sigptr);
  //kill(getppid(), SIGUSR2);

  while(1){
    //Wait for a signal to arrive and take appropriate action
    printf("waining for command %d\n",getpid());
    sigwait(&set,sigptr);

    printf("process = %d active\n",getpid());

    //If signal is SIGUSR1 update directories
    if(*sigptr == SIGUSR1){
      //Check directories to see if anything changed
      if(dir_hash_update_directories(visited_dir, records, citizen_records) == 0){
        printf("NOTHING NEW\n");
        kill(getppid(), SIGUSR2);
        continue;
      }

      sigset_t set2;
      int sig2;
      int *sigptr2 = &sig2;

      sigemptyset(&set2);
      sigaddset(&set2, SIGUSR1);

      kill(getppid(),SIGUSR2);

      int temp_size = 0;

      //Same as read fifo()
      for(int i = 0 ; i < get_hashtable_size() ; i++){
        if(records[i].start != NULL){
          HNode traverse = records[i].start;
          while(traverse){

            //Inform parent about the size of the variable we are going to send
            temp_size = strlen(traverse->disease);
            if((write(write_fifo_pipe, &temp_size, sizeof(int)) == -1)){
              perror("147");
            }
            sigwait(&set2, sigptr2);

            //Send virus name to parent
            if((write(write_fifo_pipe, traverse->disease, sizeof(char)*strlen(traverse->disease)))==-1){
              perror("153");
            }
            
            kill(getppid(), SIGUSR1);
            while(received == 0);
            received = 0;
            temp_size = get_bloom_size();

            //Inform the parent of the size of what is going to be transfered
            if((write(write_fifo_pipe, &temp_size, sizeof(int)) == -1)){
              perror("166");
            }
            kill(getppid(), SIGUSR1);
            while(received == 0);
            received = 0;

            //If size is not enough to block fifo send it with one write
            if(temp_size <= buffer && temp_size <= 10000){
              if((write(write_fifo_pipe, traverse->vaccinated->filter,sizeof(int) * temp_size) == -1)){
                perror("178 :");
              }
            }else{
              //else if size is very large and its going to block fifo file
              //cut it to smaller writes

              int div = temp_size / buffer;
              int mod = temp_size % buffer;

              for(int j = 0 ; j < div ; j++){
                //write one part of the array each time
                if(write(write_fifo_pipe, &(traverse->vaccinated->filter[j*buffer]),sizeof(int)*buffer) == -1){
                  perror("190");
                }
                kill(getppid(), SIGUSR1);
                while(received == 0);
                received = 0;
              }

              if(mod > 0){
                //write the remaining cells of the array
                if(write(write_fifo_pipe, &(traverse->vaccinated->filter[div]),sizeof(int)*mod) == -1){
                  perror("203");
                }
              }
            }
            kill(getppid(), SIGUSR1);
            
            printf("Data written %d\n",getpid());

            traverse = traverse->link;
          }
        }
      }
    
    //Else if SIGINT OR SIGQUIT create logfil and terminate
    }else if(*sigptr == SIGINT || *sigptr == SIGQUIT){
      int size = 0;
      char** countries = dir_hash_return_countries(visited_dir, &size);

      logger(getpid(),countries, size, requests, accepted, rejected);
      break;
    }else if(*sigptr == SIGUSR2){
      //if SIGUSR2 read fifo file and take appropriate action

      char buf[buffer];
      char** arr;
      int arr_size = 0;
      int n = 0;
      char* saveptr;

      char temp_buf[100];

      sprintf(temp_buf, "%d", getpid());

      //Create temporary file to read instruction
      char* temp_file = malloc(strlen(temp_buf) + strlen("temp") + strlen(".txt") + 1);
      strcpy(temp_file,"temp");
      strcat(temp_file,temp_buf);
      strcat(temp_file,".txt");

      FILE* fp = fopen(temp_file, "w+");
      if(fp == NULL){
        perror("Line 208: ");
      }
      int fd = fileno(fp);

      //write fifo containts into read file
      while((n = read(read_fifo_pipe, buf, buffer)) > 0){
        write(fd, buf, n);
      }

      fclose(fp);
      close(fd);
      
      fp = fopen(temp_file, "r+");
      if(fp == NULL){
        perror("Line 224: ");
      }

      char line[1024];
      size_t len = 1024;
      char* copy = NULL;

      fgets(line, len, fp);

      fclose(fp);
      //delete temp file
      unlink(temp_file);
      free(temp_file);
      
      copy = malloc(strlen(line) + 1);
      strcpy(copy,line);

      //cut instruction into sub strings
      char* token = strtok_r(copy, " ", &saveptr);
      arr_size++;
      arr = malloc(sizeof(char*) * arr_size);
      arr[0] = malloc(strlen(token) + 1);
      strcpy(arr[0], token);

      while(token){
        token = strtok_r(NULL," ",&saveptr);
        if(token == NULL) break;

        arr_size++;
        arr = realloc(arr, sizeof(char*) * arr_size);
        arr[arr_size - 1] = malloc(sizeof(token) + 1);
        strcpy(arr[arr_size - 1], token);
      }

      //if instruction is /travelRequest
      if(!strcmp(arr[0],"/travelRequest")){
        char* answer;

        //if there are more than 4 substring in array there is an error
        if(arr_size == 4){
          requests++;
          answer = vaccineStatus(records, arr[1], arr[3], arr[2], write_fifo_pipe);
          if(!strcmp(answer,"YES")){
            accepted++;
          }else{
            rejected++;
          }
        }else{
          answer = "error";
          write(write_fifo_pipe, answer, strlen(answer));
        }

        //Inform parent answer is ready
        kill(getppid(), SIGUSR2);
        
      }else if(!strcmp(arr[0], "/searchVaccinationStatus")){
        //If instruction is seachVaccinationStatus
        //check if id exists else continue
        if(citizen_hash_search_id(citizen_records,arr[1]) == 1){  
          citizenStatus(records, citizen_records, arr[1], write_fifo_pipe);
          
        }else{
          continue;
        }

      }else{
        break;
      }
      
    }
  }

  dir_hash_free(visited_dir);
  hash_free(records);
  citizen_hash_free(citizen_records);
  close(write_fifo_pipe);
  close(read_fifo_pipe);

  return 0;
}

//Initialize the read file where the directories that proccess is
//is responsible for are
void initialize_read_file(void){
  if(!strcmp(prev_id,"0")){
    //If its the first time a process is created,create a read file
    //that has number of the process as name
    char buf[100];

    sprintf(buf, "%d", getpid());

    read_file = malloc(strlen(buf) + strlen(".txt") + 1);
    strcpy(read_file,buf);
    strcat(read_file,".txt");
  }else{
    //Previous process was terminated so read from existing file
    read_file = malloc(strlen(prev_id) + strlen(".txt") + 1);
    strcpy(read_file,prev_id);
    strcat(read_file,".txt");
  }

}

//Read the fifo file to initialize structs and return bloom filters
void read_fifo(){
  if(read_fifo_pipe == -1) return;
  FILE* fp = NULL;

  //If its the first time write to the read file;
  if(first_time){
    sigwait(&set,sigptr);

    char buf[buffer];
    int n = 0;

    fp = fopen(read_file, "w+");
    if(fp == NULL){
      perror("Line 287: ");
    }
    int fd1 = fileno(fp);

    while((n = read(read_fifo_pipe, buf, buffer)) > 0){
      write(fd1, buf, n);
    }

    fclose(fp);
    close(fd1);
  }

  fp = fopen(read_file, "r+");
  if(fp == NULL){
    perror("Line 389: ");
  }


  char line[1024];
  int len = 1024;
  DIR* pdir = NULL;
  char* temp = NULL;

  while(1){
    fgets(line, len, fp);
    if(feof(fp)) break;

    char* token = strtok(line,"\n");

    //create path to open directory
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    temp = malloc(strlen(cwd) + strlen(token) + strlen("/") + 1);
    strcpy(temp,cwd);
    strcat(temp,"/");
    strcat(temp,token);

    char* copy = malloc(strlen(temp) + 1);
    strcpy(copy,temp);

    pdir = opendir(temp);

    if(pdir != NULL){
      //Insert directory to directory hash table
      dir_hash_insert(visited_dir,copy);

      struct stat F;
      struct dirent *dir = NULL;
      int fd = -1;

      while((dir = readdir(pdir)) != NULL){
        if(!strcmp(dir->d_name, "."))
          continue;
        if(!strcmp(dir->d_name, ".."))    
          continue;

        char* path = malloc(strlen(temp) + strlen(dir->d_name) + 2);
        strcpy(path,temp);
        strcat(path,"/");
        strcat(path,dir->d_name);

        //Insert new file to directory hash table.If its not a new file continue
        if(dir_hash_search_insert(visited_dir,temp,dir->d_name) == 0){
          free(path);
          continue;
        }

        //to find information about the file
        stat(path, &F);
        
        if((fd = open(path, O_RDONLY)) == -1){
          perror("447 file :");
          continue;
        }

        fd_size = F.st_size;

        if((mmap_file = mmap(NULL, F.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
          perror("line 454 : mmap ");
          exit(1);
        } 

        //Parse file
        parser(records, citizen_records ,mmap_file, F.st_size);
        munmap(mmap_file, F.st_size);
        //hash_print(records);

        close(fd);
        free(path);

      }

      free(temp);
      temp = NULL;
      closedir(pdir);
    }

  }

  sigset_t set2;
  int sig2;
  int *sigptr2 = &sig2;

  sigemptyset(&set2);
	sigaddset(&set2, SIGUSR1);

  //kill(getppid(),SIGUSR2);

  
  fclose(fp);
  int temp_size = 0;

  //If its the first time the process was created, send information to parent
  //else skip
  if(first_time){
    
    //Same as when receiving a SIGUSR1 signal (line 138 same commends)
    for(int i = 0 ; i < get_hashtable_size() ; i++){
      if(records[i].start != NULL){
        HNode traverse = records[i].start;
        while(traverse){

          temp_size = strlen(traverse->disease);
          if((write(write_fifo_pipe, &temp_size, sizeof(int)) == -1)){
            perror("500");
          }

          sigwait(&set2, sigptr2);
          if((write(write_fifo_pipe, traverse->disease, sizeof(char)*strlen(traverse->disease)))==-1){
            perror("505");
          }
          
          kill(getppid(), SIGUSR1);

          while(received == 0);
          received = 0;
          temp_size = get_bloom_size();

          if((write(write_fifo_pipe, &temp_size, sizeof(int)) == -1)){
            perror("517");
          }
          kill(getppid(), SIGUSR1);
          
          while(received == 0);
          received = 0;

          if(temp_size <= buffer && temp_size <= 10000){
            if((write(write_fifo_pipe, traverse->vaccinated->filter,sizeof(int) * temp_size) == -1)){
              perror("528 :");
            }
          }else{
            int div = temp_size / buffer;
            int mod = temp_size % buffer;

            for(int j = 0 ; j < div ; j++){
              if(write(write_fifo_pipe, &(traverse->vaccinated->filter[j*buffer]),sizeof(int)*buffer) == -1){
                perror("536");
              }

              kill(getppid(), SIGUSR1);
              while(received == 0);
              received = 0;
              
            }

            if(mod > 0){
              if(write(write_fifo_pipe, &(traverse->vaccinated->filter[div]),sizeof(int)*mod) == -1){
                perror("549");
              }
            }
          }
          kill(getppid(), SIGUSR1);
          
          printf("Data written %d\n",getpid());

          traverse = traverse->link;
        }
      }
    }

  }

}

//The signal handler for Monitor
void signal_handler(int sig){

  switch(sig){
    case SIGINT:
    case SIGQUIT:
    {
      int size = 0;
      //Get countries that process was responsible for.
      char** countries = dir_hash_return_countries(visited_dir, &size);

      //Send info to logger and create log file
      logger(getpid(),countries, size, requests, accepted, rejected);

      //free memory allocated from process(not needed)
      for(int i = 0 ; i < size ; i++){
        free(countries[i]);
      }
      free(countries);

      hash_free(records);
      citizen_hash_free(citizen_records);
      dir_hash_free(visited_dir);
      free_array_contents(array, 8);
      if(read_fifo_pipe != -1) close(read_fifo_pipe);
      if(write_fifo_pipe != -1) close(write_fifo_pipe);
      if(parser_token){
        free(parser_token);
        munmap(mmap_file, fd_size);
      }
      exit(0);
      break;
    }
    case SIGUSR1:
      received =1;
      break;
    case SIGUSR2:
      waiting--;
      break;
    case SIGSEGV:
      printf("%d Segmentation fault\n",getpid());
      exit(1);
      break;
    default:
      break;
  }

	return;
}

//parses file and inserts records to structures
void parser(HTable records, CHTable citizen_records, char* word, int size){
  
  int array_size = 8;
  array = malloc(sizeof(char*) * array_size);

  for(int i = 0 ; i < array_size ; i++){
    array[i] = NULL;
  }

  int word_size = 0;
  int counter = 0;
  int flag = 0;         //error flag 

  for(int i = 0 ; i < size ; i++){
    if(flag){
      if(word[i] == '\n'){
        //if all row is read
        flag = 0;
        counter = 0;
      }else{
        //ignore until new row
        continue;
      }
    }
    else if(word[i] == ' ' && parser_token == NULL){
      //if white space and no word
      continue;
    }else if((word[i] == ' ' || word[i] == '\n') && parser_token != NULL){
      //if word is complete
      word_size = 0;

      //depending on the number of words already read in a row pick a case
      switch (counter){
        case 0:
        //check id
          if(is_integer(parser_token)){
            array[counter] = parser_token;
            parser_token = NULL;
          }else{
            //printf("error with record id\n");
            flag = 1;
          }
          break;
        case 1:
        //check if first name is string
          if(is_string(parser_token)){
            array[counter] = parser_token;
            parser_token = NULL;
          }else{
            //printf("error with record first name\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 2:
        //check if last name is string
          if(is_string(parser_token)){
            array[counter] = parser_token;
            parser_token = NULL;
          }else{
            //printf("error with record last name\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 3:
        //check if country is string
          if(is_string(parser_token)){
            array[counter] = parser_token;
            parser_token = NULL;
          }else{
            //printf("error with record country\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 4:
        //check if age is integer and in valid range
          if(is_integer(parser_token) && is_valid_age(parser_token)){
            array[counter] = parser_token;
            parser_token = NULL;
          }else{
            //printf("error with record age %s\n", parser_token);
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 5:
        //can be alpharithmetic so no need to check anything
          array[counter] = parser_token;
          parser_token = NULL;
          break;
        case 6:
        //check if answer is yes or no
          if(yes_or_no(parser_token)){
            array[counter] = parser_token;
            parser_token = NULL;
            //if no record is ready. Insert it to structs
            if(!strcmp(array[counter],"NO")){
              hash_insert(records, array);
              citizen_hash_insert(citizen_records, array);
            }
          }else{
            //printf("error with record answer\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 7:
        //Check if previous string was yes
          if(!strcmp(array[counter - 1], "YES")){
            //check if date is valid
            if(is_date(parser_token)){
              array[counter] = parser_token;
              parser_token = NULL;
            }else{
             // printf("error with record date\n");
              flag = 1;
              free_array_contents(array, array_size);
            }
          }else{
            //printf("error with record answer/date %s\n",parser_token);
            flag = 1;
            free_array_contents(array, array_size);
          }
          //if no errors and all words are read, insert record to structs
          if(!flag){
            //if id already exists
            if(citizen_hash_search(citizen_records, array) == 1){
              hash_insert(records, array);
            }else if(citizen_hash_search(citizen_records, array) == 2){
              //if id is new
              citizen_hash_insert(citizen_records, array);
              hash_insert(records, array);
            }else{
              //if id and info do not match
              //printf("error with record. Records with id %s exists with other name/lastname/age/country\n",array[citizen]);
              free_array_contents(array, array_size);
            }
          }
          break;
        default:
          break;

      }
      counter++;
      //if record is complete and inserted,reset values for new records.
      if(counter == 8 || (counter == 7 && !strcmp(array[yes_no],"NO"))){
        free_array_contents(array, array_size);
        counter = 0;
      }
    }else if(word[i] == '\n'){
      //if new line before neccesary info delete containts
      if(counter < 7){
        free_array_contents(array, array_size);
        //printf("Error with record.Incomplete\n");
      }
    }else{
      //if new word create it.
      if(parser_token == NULL){
        word_size++;
        parser_token = malloc(sizeof(char)*word_size + 1);
				parser_token[0] = word[i];
				parser_token[1] = '\0';
      }else{
        //else expand it until whitespace
        word_size++;
				parser_token = realloc(parser_token,strlen(parser_token) + 2);
				parser_token[word_size-1] = word[i];
				parser_token[word_size] = '\0';
      }
    }
  }

  free(array);
}

//deallocate memory from an array
void free_array_contents(char** arr, int size){
  if(arr == NULL) return;
  for(int j = 0 ; j < size ; j++){
    if(arr[j] != NULL) free(arr[j]);
    arr[j] = NULL;
  }
}