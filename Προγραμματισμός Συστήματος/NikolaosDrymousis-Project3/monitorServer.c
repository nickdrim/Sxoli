#include "header.h"

//GLOBAL VARIABLES
//They are global because the might be needed in multiple
//functions and the signal handler in case of signal
//from parent
int waiting = 0;
int received = 0;


struct sockaddr_in server_adr;
int socket_fd = -1;
int buffer = 0;
int bloom_size = 0;
int no_threads = 0;
int cyclic_buffer_size = 0;
int port = -1;

pthread_t* thread_ids = NULL;

pthread_mutex_t mtx;
pthread_mutex_t parser_mtx;
pthread_mutex_t dir_mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
int termination_flag = 0;

//Structures we will new
HTable records = NULL;
CHTable citizen_records = NULL;
DHash visited_dir = NULL;
CBUFFER round_buffer = NULL;

//The idea for them being global was that in case of a terminating signal
//they would be freed by the handler but since we dont need to do that
//i just left them here

//Variables that count all the requests that this process has received
int requests = 0;
int accepted = 0;
int rejected = 0;

//Variables for sigwait
sigset_t set;
int sig;
int *sigptr = &sig;

void check_host_name(int hostname) { //This function returns host name for local computer
   if (hostname == -1) {
      perror("gethostname");
      exit(1);
   }
}
void check_host_entry(struct hostent * hostentry) { //find host info from host name
   if (hostentry == NULL){
      perror("gethostbyname");
      exit(1);
   }
}
void IP_formatter(char *IPbuffer) { //convert IP string to dotted decimal format
   if (NULL == IPbuffer) {
      perror("inet_ntoa");
      exit(1);
   }
}

int main(int argc, char *argv[]){
  srand(time(NULL));

  printf("PROCCESS %d started\n",getpid());

  //Initialize bloom size buffer size and if its the first time procces start
  port = atoi(argv[1]);
  no_threads = atoi(argv[2]);
  buffer = atoi(argv[3]);
  cyclic_buffer_size = atoi(argv[4]);
  bloom_size = atoi(argv[5]);

  //create socket
  if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Line 72 socket:");
    exit(1);
  }

  //custom settings
  int truer = 1;
  setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&truer,sizeof(int));

  //find local IP
  char host[256];
  char *IP;
  struct hostent *host_entry;
  int hostname;
  hostname = gethostname(host, sizeof(host)); //find the host name
  check_host_name(hostname);
  host_entry = gethostbyname(host); //find host information
  check_host_entry(host_entry);
  IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

  bzero(&server_adr, sizeof(server_adr));
  server_adr.sin_family = AF_INET;
  server_adr.sin_port = htons(port);

  if(inet_pton(AF_INET, IP, &server_adr.sin_addr) <= 0){
    perror("Line 155 inet_pton");
    exit(1);
  }

  if(connect(socket_fd, (struct sockaddr *) &server_adr, sizeof(server_adr)) < 0){
    perror("line 110 connect:");
    exit(1);
  }

  //Initialize structures
  records = hash_initialize(bloom_size);
  citizen_records = citizen_hash_initialize();
  visited_dir = dir_hash_initialize();
  round_buffer = initialize_cyclic_buffer(cyclic_buffer_size);


  //initialize mutexes and conditions
  pthread_mutex_init(&mtx, 0);
  pthread_mutex_init(&parser_mtx, 0);
  pthread_mutex_init(&dir_mtx, 0);
  pthread_cond_init(&cond_nonempty, 0);
  pthread_cond_init(&cond_nonfull, 0);


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
  kill(getppid(), SIGUSR1);
  
  //Create threads
  thread_ids = malloc(sizeof(pthread_t) * no_threads);
  
  for(int i = 0 ; i < no_threads ; i++){
    pthread_create(&thread_ids[i], 0 , thread_function, 0);
  }

  //add to cyclic buffer
  for(int i = 6 ; i < argc ; i++){
    add_to_buffer(argv[i]);

    //signal threads that buffer is not empty
    pthread_cond_signal(&cond_nonempty);
  }

  //unblock all threads
  pthread_cond_broadcast(&cond_nonempty);
  termination_flag = 1;

  for(int i = 0 ; i < no_threads ; i++){
    pthread_join(thread_ids[i], 0);
  }

  kill(getppid(),SIGUSR2);
  //read the fifo file(Line 366 for function)
  send_info();

  free(thread_ids);
  printf("proccess %d ready\n",getpid());
  
  //wait for parent procces to read information
  //from all proccesses before continuing 

  while(1){
    int flag = 0;
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

      //Same as send info()
      for(int i = 0 ; i < get_hashtable_size() ; i++){
        if(records[i].start != NULL){
          HNode traverse = records[i].start;
          while(traverse){

            //Inform parent about the size of the variable we are going to send
            temp_size = strlen(traverse->disease);
            if((write(socket_fd, &temp_size, sizeof(int)) == -1)){
              perror("147");
            }
            sigwait(&set2, sigptr2);

            //Send virus name to parent
            if((write(socket_fd, traverse->disease, sizeof(char)*strlen(traverse->disease)))==-1){
              perror("153");
            }
            
            kill(getppid(), SIGUSR1);
            while(received == 0);
            received = 0;
            temp_size = get_bloom_size();

            //Inform the parent of the size of what is going to be transfered
            if((write(socket_fd, &temp_size, sizeof(int)) == -1)){
              perror("166");
            }
            kill(getppid(), SIGUSR1);
            while(received == 0);
            received = 0;

            //If size is not enough to block fifo send it with one write
            if(temp_size <= buffer && temp_size <= 10000){
              if((write(socket_fd, traverse->vaccinated->filter,sizeof(int) * temp_size) == -1)){
                perror("178 :");
              }
            }else{
              //else if size is very large and its going to block fifo file
              //cut it to smaller writes

              int div = temp_size / buffer;
              int mod = temp_size % buffer;

              for(int j = 0 ; j < div ; j++){
                //write one part of the array each time
                if(write(socket_fd, &(traverse->vaccinated->filter[j*buffer]),sizeof(int)*buffer) == -1){
                  perror("190");
                }
                kill(getppid(), SIGUSR1);
                while(received == 0);
                received = 0;
              }

              if(mod > 0){
                //write the remaining cells of the array
                if(write(socket_fd, &(traverse->vaccinated->filter[div]),sizeof(int)*mod) == -1){
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
      while((n = read(socket_fd, buf, buffer)) > 0){
        write(fd, buf, n);
        if(n < buffer) break;
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
        arr[arr_size - 1] = malloc(strlen(token) + 1);
        strcpy(arr[arr_size - 1], token);
      }

      free(copy);

      //if instruction is /travelRequest
      if(!strcmp(arr[0],"/travelRequest")){
        char* answer;

        //if there are more than 4 substring in array there is an error
        if(arr_size == 4){
          requests++;
          answer = vaccineStatus(records, arr[1], arr[3], arr[2], socket_fd);
          if(!strcmp(answer,"YES")){
            accepted++;
          }else{
            rejected++;
          }
        }else{
          answer = "error";
          int num = strlen("error");
          write(socket_fd, &num, sizeof(int));
          write(socket_fd, answer, strlen(answer));
        }

        //Inform parent answer is ready
        kill(getppid(), SIGUSR2);
        
      }else if(!strcmp(arr[0], "/searchVaccinationStatus")){
        //If instruction is seachVaccinationStatus
        //check if id exists else continue
        if(citizen_hash_search_id(citizen_records,arr[1]) == 1){  
          citizenStatus(records, citizen_records, arr[1], socket_fd);
          
        }

      }else if(!strcmp(arr[0],"/exit")){
        flag = 1;
      }

      for(int i = 0 ; i < arr_size ; i++){
        free(arr[i]);
      }

      free(arr);
      
      if(flag) break;
    }
  }

  int size = 0;
  char** countries = dir_hash_return_countries(visited_dir, &size);

  if(countries){
    logger(getpid(),countries, size, requests, accepted, rejected);

    for(int i = 0 ; i < size ; i++){
      free(countries[i]);
    }
    free(countries);
  }
  //Destroy conditions and mutexes
  pthread_cond_destroy(&cond_nonempty);
  pthread_cond_destroy(&cond_nonfull);
  pthread_mutex_destroy(&mtx);
  pthread_mutex_destroy(&dir_mtx);
  pthread_mutex_destroy(&parser_mtx);

  dir_hash_free(visited_dir);
  hash_free(records);
  citizen_hash_free(citizen_records);
  free_cyclic_buffer();
  close(socket_fd);

  return 0;
}

//Read the fifo file to initialize structs and return bloom filters
void send_info(){

  sigset_t set2;
  int sig2;
  int *sigptr2 = &sig2;

  sigemptyset(&set2);
	sigaddset(&set2, SIGUSR1);
  int temp_size = 0;

  //Same as when receiving a SIGUSR1 signal (line 138 same commends)
  for(int i = 0 ; i < get_hashtable_size() ; i++){
    if(records[i].start != NULL){
      HNode traverse = records[i].start;
      while(traverse){

        temp_size = strlen(traverse->disease);
        if((write(socket_fd, &temp_size, sizeof(int)) == -1)){
          perror("386");
        }

        sigwait(&set2, sigptr2);
        if((write(socket_fd, traverse->disease, sizeof(char)*strlen(traverse->disease)))==-1){
          perror("391");
        }
        
        kill(getppid(), SIGUSR1);

        while(received == 0);
        received = 0;
        temp_size = get_bloom_size();

        if((write(socket_fd, &temp_size, sizeof(int)) == -1)){
          perror("401");
        }
        kill(getppid(), SIGUSR1);
        
        while(received == 0);
        received = 0;

        if(temp_size <= buffer && temp_size <= 10000){
          if((write(socket_fd, traverse->vaccinated->filter,sizeof(int) * temp_size) == -1)){
            perror("410 :");
          }
        }else{
          int div = temp_size / buffer;
          int mod = temp_size % buffer;

          for(int j = 0 ; j < div ; j++){
            if(write(socket_fd, &(traverse->vaccinated->filter[j*buffer]),sizeof(int)*buffer) == -1){
              perror("418");
            }

            kill(getppid(), SIGUSR1);
            while(received == 0);
            received = 0;
            
          }

          if(mod > 0){
            if(write(socket_fd, &(traverse->vaccinated->filter[div]),sizeof(int)*mod) == -1){
              perror("429");
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

void* thread_function(void *ptr){

  //while there are more paths to add(termination flag)
  //and the buffer is not empty
  while(termination_flag != 1 || round_buffer->count > 0){

    char* mmap_file = NULL;
    char* tem_tok = remove_from_buffer();
    pthread_cond_signal(&cond_nonfull);

    if(tem_tok == NULL) break;

    char* token = malloc(strlen(tem_tok) + 1);
    strcpy(token, tem_tok);
    //pthread_cond_signal(&cond_nonfull);


    DIR* pdir = NULL;
    char* temp = NULL;

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
      pthread_mutex_lock(&dir_mtx);
      dir_hash_insert(visited_dir,copy);
      pthread_mutex_unlock(&dir_mtx);

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


        //printf("checking lock %d\n",pthread_self());
        pthread_mutex_lock(&dir_mtx);
        //Insert new file to directory hash table.If its not a new file continue
        if(dir_hash_search_insert(visited_dir,temp,dir->d_name) == 0){
          free(path);
          free(temp);
          pthread_mutex_unlock(&dir_mtx);
          continue;
        }
        pthread_mutex_unlock(&dir_mtx);

        //to find information about the file
        stat(path, &F);
        //printf("path -> %s\n",path);
        
        if((fd = open(path, O_RDONLY)) == -1){
          perror("447 file :");
          continue;
        }

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
      free(copy);
      temp = NULL;
      closedir(pdir);
    }

    free(token);
  }

  pthread_exit(0);

}

//Initialize the cyclic buffer
CBUFFER initialize_cyclic_buffer(int size){
    if(size == 0){
        printf("Size of cyclic buffer cant be 0\n");
        return NULL;
    }

    CBUFFER temp = malloc(sizeof(cyclic_buffer));
    temp->buffer_size = size;
    temp->cyclic = malloc(sizeof(char*) * size);
    temp->end = -1;
    temp->start = 0;
    temp->count = 0;

    for(int i = 0 ; i < size ; i++){
        temp->cyclic[i] = NULL;
    }

    return temp;

}

//add path to cyclic buffer
void add_to_buffer(char* word){
    if(word == NULL || round_buffer == NULL) return;

    pthread_mutex_lock(&mtx);

    //wait until buffer is not full
    while(round_buffer->count >= round_buffer->buffer_size){
        pthread_cond_wait(&cond_nonfull, &mtx);
    }

    round_buffer->end = (round_buffer->end + 1) % round_buffer->buffer_size;
    round_buffer->cyclic[round_buffer->end] = word;
    round_buffer->count++;
    printf("%d Added to buffer\n",getpid());
    pthread_mutex_unlock(&mtx);

}

//remove path from cyclic buffer
char* remove_from_buffer(void){
    char* data = NULL;
    pthread_mutex_lock(&mtx);

    //if buffer is empty and no more paths to add return
    if(termination_flag == 1 && round_buffer->count == 0){
      pthread_mutex_unlock(&mtx);
      return NULL;
    }

    //Wait until item is added to buffer
    while(round_buffer->count <= 0){
        pthread_cond_wait(&cond_nonempty, &mtx);
    }

    data = round_buffer->cyclic[round_buffer->start];
    round_buffer->cyclic[round_buffer->start] = NULL;
    round_buffer->start = (round_buffer->start + 1) % round_buffer->buffer_size;
    round_buffer->count--;
    printf("%d Removed from buffer\n",getpid());
    pthread_mutex_unlock(&mtx);

    return data;
}

//free the cyclic buffer
void free_cyclic_buffer(void){
  if(round_buffer == NULL) return;

  free(round_buffer->cyclic);
  free(round_buffer);
}

//The signal handler for Monitor
void signal_handler(int sig){

  switch(sig){
    case SIGINT:
    case SIGQUIT:
    {
      printf("Child process with id = %d terminating\n",getpid());
      int size = 0;
      char** countries = dir_hash_return_countries(visited_dir, &size);

      if(countries){
        logger(getpid(),countries, size, requests, accepted, rejected);

        for(int i = 0 ; i < size ; i++){
          free(countries[i]);
        }
        free(countries);
      }

      pthread_cond_destroy(&cond_nonempty);
      pthread_cond_destroy(&cond_nonfull);
      pthread_mutex_destroy(&mtx);
      pthread_mutex_destroy(&dir_mtx);
      pthread_mutex_destroy(&parser_mtx);

      dir_hash_free(visited_dir);
      hash_free(records);
      citizen_hash_free(citizen_records);
      free_cyclic_buffer();
      close(socket_fd);

      exit(1);
    }
    case SIGUSR1:
      received =1;
      break;
    case SIGUSR2:
      waiting--;
      break;
    case SIGSEGV:
      printf("%d Segmentation fault\n",getpid());

      close(socket_fd);
      exit(1);
      break;
    default:
      break;
  }

  return;
}

//parses file and inserts records to structures
void parser(HTable records, CHTable citizen_records, char* word, int size){

  char** array = NULL;

  int array_size = 8;
  array = malloc(sizeof(char*) * array_size);

  for(int i = 0 ; i < array_size ; i++){
    array[i] = NULL;
  }

  char* parser_token = NULL;
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
    }else if(word[i] == ' ' && parser_token == NULL){
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
            free(parser_token);
            parser_token = NULL;
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
            free(parser_token);
            parser_token = NULL;
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
            free(parser_token);
            parser_token = NULL;
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
            free(parser_token);
            parser_token = NULL;
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
            free(parser_token);
            parser_token = NULL;
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
              pthread_mutex_lock(&parser_mtx);
              hash_insert(records, array);
              citizen_hash_insert(citizen_records, array);
              pthread_mutex_unlock(&parser_mtx);
            }
          }else{
            //printf("error with record answer\n");
            flag = 1;
            free_array_contents(array, array_size);
            free(parser_token);
            parser_token = NULL;
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
              free(parser_token);
              parser_token = NULL;
            }
          }else{
            //printf("error with record answer/date %s\n",parser_token);
            flag = 1;
            free_array_contents(array, array_size);
            free(parser_token);
            parser_token = NULL;
          }
          //if no errors and all words are read, insert record to structs
          if(!flag){
            //if id already exists
            if(citizen_hash_search(citizen_records, array) == 1){
              pthread_mutex_lock(&parser_mtx);
              hash_insert(records, array);
              pthread_mutex_unlock(&parser_mtx);
            }else if(citizen_hash_search(citizen_records, array) == 2){
              pthread_mutex_lock(&parser_mtx);
              //if id is new
              citizen_hash_insert(citizen_records, array);
              hash_insert(records, array);
              pthread_mutex_unlock(&parser_mtx);
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
        parser_token = NULL;
      }
    }else if(word[i] == '\n'){
      //if new line before neccesary info delete containts
      if(counter < 7){
        free_array_contents(array, array_size);
        
        if(parser_token) free(parser_token);
        parser_token = NULL;
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
				parser_token = realloc(parser_token,word_size + 1);
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
    if(arr[j] != NULL){
      free(arr[j]);
    }
    arr[j] = NULL;
  }
}