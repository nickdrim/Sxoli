#include "header.h"

//GLOBAL VARIABLES
//They are global because the might be needed in multiple
//functions and the signal handler in case of signal
//from child processes or SIGINT etc

//Argument array for child processes
char*** argument_array = NULL;

//number of arguments each array contains
int* argument_counter = NULL;

int initialization = 0;
int termination_flag = 0;

DIR* working_dir = NULL;
char* file = NULL;
char* dir_name = NULL;

//bloom filter info
int bloom_size = 0;
char* bloom_string = NULL; //pass as argument to exec

//number of child process
int no_childs = 0;

//number of threads
char* no_threads = NULL;

//cyclic buffer size
char* cyclic_buffer_size = NULL;

//Buffer size info
int buffer = 0;
char* buffer_string = NULL; //pass as argument to exec

//Array to store the ids of child processes
int* pids = NULL;

//socket
int server = -1;

//Array for socket fds
int* socket_fds = NULL;

//Structures we will need
PIDHASH records = NULL;
catalog* country_responsible_pid = NULL;
int responsible_size = 0;

//variables that store the total number of requests, total number of accepted requests
//and total number of rejected requests.These are send to logger
int accepted = 0;
int requests = 0;
int rejected = 0;

//flags regarding child processes and signals
int waiting = 0;
int received = 0;

//Signal set for sigwait
sigset_t set;
int sig;
int *sigptr = &sig;

struct pollfd* global_pfds;

//declarations of local functions
void free_global_variables(void);
void initialize_global_variables(void);

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
  //checks if arguments are correct
  if(argument_check(argc, argv)){
    printf("Run command is as follows:\n");
    printf("./travelMonitorClient –m numMonitors -b bufferSize -c cyclicBufferSize -s sizeOfBloom -i input_dir -t numThreads");
    exit(1);
  }

  //Initialize signal set
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGCHLD);

  //Initialize signal handler
  struct sigaction sa = {0};
	sa.sa_handler = monitor_signal_handler;

	sigaction(SIGINT, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGCHLD, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);

  initialize_global_variables();

  struct pollfd pfds[no_childs];
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  waiting = no_childs;

  char buf[buffer];

  argument_array = malloc(sizeof(char**) * no_childs);
  argument_counter = malloc(sizeof(int) * no_childs);

  char temp_buf[20];
  sprintf(temp_buf , "%d", PORT);
  char* port = malloc(strlen(temp_buf) + 1);
  strcpy(port,temp_buf);

  //Initialize argument arrays
  for(int i = 0 ; i < no_childs ; i++){

    argument_array[i] = malloc(sizeof(char*) * 6);
    argument_array[i][0] = malloc(strlen("./monitorServer") + 1);
    strcpy(argument_array[i][0],"./monitorServer");
    argument_array[i][1] = malloc(strlen(port) + 1);
    strcpy(argument_array[i][1],port);
    argument_array[i][2] = malloc(strlen(no_threads) + 1);
    strcpy(argument_array[i][2], no_threads);
    argument_array[i][3] = malloc(strlen(buffer_string) + 1);
    strcpy(argument_array[i][3], buffer_string);
    argument_array[i][4] = malloc(strlen(cyclic_buffer_size) + 1);
    strcpy(argument_array[i][4], cyclic_buffer_size);
    argument_array[i][5] = malloc(strlen(bloom_string) + 1);
    strcpy(argument_array[i][5], bloom_string);

    argument_counter[i] = 6;

  }
  free(port);

  //create socket
  if((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Line 142 socket: ");
    exit(1);
  }

  //custom settings
  int truer = 1;
  setsockopt(server,SOL_SOCKET,SO_REUSEADDR,&truer,sizeof(int));

  //Find local address
  char host[256];
  char *IP;
  struct hostent *host_entry;
  int hostname;
  hostname = gethostname(host, sizeof(host)); //find the host name
  check_host_name(hostname);
  host_entry = gethostbyname(host); //find host information
  check_host_entry(host_entry);
  IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);

  if(inet_pton(AF_INET, IP, &address.sin_addr) <= 0){
    perror("Line 155 inet_pton");
    exit(1);
  }

  if(bind(server,(struct sockaddr *)&address, sizeof(address)) < 0){
      perror("Line 148 bind failed");
      exit(1);
  }

  if(listen(server, no_childs)){
    perror("Line 153 listen:");
    exit(1);
  }


  struct dirent *dir = NULL;
  //send work load equally to proccesses
  int rrobin = 0;

  //navigate directories
  while((dir = readdir(working_dir)) != NULL){
    if(!strcmp(dir->d_name, "."))
      continue;
    if(!strcmp(dir->d_name, ".."))    
      continue;
  
    //create fullpath of file
    char* fullpath = malloc(strlen(dir_name) + strlen(dir->d_name) + 2);
    strcpy(fullpath, dir_name);
    strcat(fullpath,"/");
    strcat(fullpath,dir->d_name);

    //restart indexes
    if(rrobin > no_childs - 1){
      rrobin = 0;
    }

    //Add country to catalogs and associate it with the process id that is responsible for it 
    responsible_size++;
    if(country_responsible_pid == NULL){
      country_responsible_pid = malloc(sizeof(catalog) * responsible_size);
      country_responsible_pid[responsible_size-1] = malloc(sizeof(country_catalog));
      country_responsible_pid[responsible_size-1]->country = malloc(strlen(dir->d_name) + 1);
      strcpy(country_responsible_pid[responsible_size-1]->country, dir->d_name);
      country_responsible_pid[responsible_size-1]->pid = rrobin;
      country_responsible_pid[responsible_size-1]->fpath = malloc(strlen(fullpath) + 1);
      strcpy(country_responsible_pid[responsible_size-1]->fpath, fullpath);
    }else{
      country_responsible_pid = realloc(country_responsible_pid,sizeof(catalog) * responsible_size);
      country_responsible_pid[responsible_size-1] = malloc(sizeof(country_catalog));
      country_responsible_pid[responsible_size-1]->country = malloc(strlen(dir->d_name) + 1);
      strcpy(country_responsible_pid[responsible_size-1]->country, dir->d_name);
      country_responsible_pid[responsible_size-1]->pid = rrobin;
      country_responsible_pid[responsible_size-1]->fpath = malloc(strlen(fullpath) + 1);
      strcpy(country_responsible_pid[responsible_size-1]->fpath, fullpath);
    }

    //add path to responsible argument array
    argument_counter[rrobin]++;
    int temp_index = argument_counter[rrobin];
    argument_array[rrobin] = realloc(argument_array[rrobin], sizeof(char*) * temp_index);
    argument_array[rrobin][temp_index-1] = malloc(strlen(fullpath) + 1);
    strcpy(argument_array[rrobin][temp_index-1], fullpath);


    rrobin++;
  

    free(fullpath);

  }

  for(int i = 0 ; i < no_childs ; i++){

    //Create temporary NULL terminated argument array for exec    
    char* argv2[argument_counter[i] + 1];
    for(int j = 0 ; j < argument_counter[i] ; j++){
      argv2[j] = malloc(strlen(argument_array[i][j])+1);
      strcpy(argv2[j],argument_array[i][j]);
    }

    argv2[argument_counter[i]] = NULL;

    int tempid = -1;
    
    if((tempid = fork()) < 0){
      perror("fork ");
      exit(1);
    }

    if(tempid == 0){
      if(execv("./monitorServer",argv2) == -1){
        perror("exec :");
        exit(1);
      }
    }else{
      //Wait for children to initialize
      sigwait(&set,sigptr);
      //In case the child terminates prematurely exit program
      if(*sigptr == SIGCHLD){
        printf("SIGCHLD before initializing.EXITING\n");
        exit(1);
      }
     
      //Accosiate array chells with process ids
      if((socket_fds[i] = accept(server, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
        perror("Line 271 accept:");
        exit(1);
      }

      pfds[i].fd = socket_fds[i];
      pfds[i].events = 0;
      pfds[i].events |= POLLIN;

      pids[i] = tempid;
      pid_hash_insert(records, tempid, i);
    }

    for(int j = 0 ; j < argument_counter[i] ; j++){
      free(argv2[j]);
    }

  }

  //Now that pids are initialized associate then with the catalog
  //and insert to pid hash table
  for(int i = 0 ; i < responsible_size ; i++){
    country_responsible_pid[i]->pid = pids[country_responsible_pid[i]->pid];

    pid_hash_search_insert(records, country_responsible_pid[i]->pid, country_responsible_pid[i]->fpath, country_responsible_pid[i]->country);
  }

  //So that they can be accessed from everywhere in the program
  global_pfds = pfds;

  //Initialize new set to accept only one signal
  sigset_t set2;

  sigemptyset(&set2);
	sigaddset(&set2, SIGUSR1);


  while(waiting);
  initialization = 1;

  int ret = 1;
  int timeout = 5000;

  //Wait until there is something to read
  while(ret > 0){
    ret = poll(global_pfds, no_childs, timeout);
  
    if(ret == -1) perror("246");

    if(ret > 0){
      for(int i = 0 ; i < no_childs ; i++){
        //if there is data to read
        if(global_pfds[i].revents == POLLIN){
          int size = 0;
          char* desease = NULL;

          //read size of the next send info
          if(read(global_pfds[i].fd, &size, sizeof(int)) == -1){
            perror("257");
          }

          //Inform child you have read first message
          kill(pids[i],SIGUSR1);
          //wait for child to write another message
          while(received == 0);
          received = 0;
          //If the word length send is smaller than the buffer
          //one read
          if(size <= buffer && size > 0){
            char temp_buf[size];

            if(read(global_pfds[i].fd, temp_buf, size) == -1){
              perror("276:");
            }

            desease = malloc(size + 1);
            for(int j = 0 ; j < size ; j++){
              desease[j] = temp_buf[j];
            }
            desease[size] = '\0';
            
          }else{
            //else if the buffer is not enough
            //Calculate how many reads it will take to rea

            int divi = size / buffer;
            int mod = size % buffer;
            desease = malloc(size + 1);
            
            for(int j = 0 ; j < divi ; j++){
              if(read(global_pfds[i].fd, buf, buffer) == -1){
                perror("294:");
              }
              if(j==0){
                
                for(int k = 0 ; k < buffer ; k++){
                  desease[k] = buf[k];
                }
                desease[(j+1)*buffer] = '\0';
              }else{
                int n = 0;
                for(int k = j*buffer ; k < (j+1)*buffer ; k++){
                  desease[k] = buf[n];
                  n++;
                }
                desease[(j+1)*buffer] = '\0';
              }
            }
            if(mod > 0){

              char temp_buf[mod];
              
              if(read(global_pfds[i].fd,temp_buf,mod) == -1){
                perror("316");
              }

              int n = 0;
              for(int j = divi * buffer ; j < divi * buffer + mod ; j++){
                desease[j] = temp_buf[n];
                n++;
              }
              desease[divi * buffer + mod] = '\0';
              
            }
          }

          //inform and wait
          kill(pids[i],SIGUSR1);
          
          while(received == 0);
          received = 0;

          //read size of bloom filter send
          if(read(global_pfds[i].fd, &size, sizeof(int)) == -1){
            perror("245");
          }
          //inform and wait
          kill(pids[i],SIGUSR1);
          
          while(received == 0);
          received = 0;

          //allocate temporary bloom
          int* temp_bloom = malloc(sizeof(int) * size);

          //if the size is smaller than the buffer and the size of the bloom filter
          //is not that large to block the pipe,read it in one go
          if(size <= buffer && size <= 10000){
            if(read(global_pfds[i].fd, temp_bloom, sizeof(int) * size) == -1){
              perror("251");
            }
          }else{
            //else calculate how many reads it will take
            int div = size / buffer;
            int mod = size % buffer;

            for(int j = 0 ; j < div ; j++){
              //continue reading from pointer
              if(read(global_pfds[i].fd, &temp_bloom[j*buffer], sizeof(int)*buffer) == -1){
                perror("273");
              }
              //inform and wait
              kill(pids[i], SIGUSR1);
              
              while(received == 0);
              received = 0;
              
            }

            if(mod > 0){
              if( read(global_pfds[i].fd, &temp_bloom[div], sizeof(int)*mod) == -1){
                perror("281");
              }
            }
            
          }

          //initialize or update bloom filter for a specific desease for all countries 
          pid_hash_initialize_bloom(records, pids[i], desease, temp_bloom);
          
          free(temp_bloom);

          free(desease);
          desease = NULL;

          //reinitialize pollfd
          global_pfds[i].fd = socket_fds[i];
          global_pfds[i].events = POLLIN;
          printf("Data read %d\n",pids[i]);
        }


      }

    }
  }

  
  //go to command line(Line 661)
  command_line();

}

//signal handler for travleMonitor
void monitor_signal_handler(int sig){
  switch (sig){
    case SIGUSR1:
      received = 1;
      break;
    case SIGUSR2:
      waiting--;
      break;
    case SIGINT:
    case SIGQUIT:
      termination_flag = 1;
      printf("Received SIGINT! Terminating child process and exiting\n");

      //kill children processes
      for(int i = 0 ; i < no_childs ; i++){
        close(socket_fds[i]);
        if(pids[i] > 0){
          if(initialization){
            kill(pids[i],SIGINT);
          }else{
            kill(pids[i],SIGKILL);
          }
        }
      }

      wait(NULL);

      free_global_variables();

      close(server);

      exit(1);
      break;
    case SIGCHLD:
      {
        if(!termination_flag){
          int temppid = 0;
          if((temppid = wait(NULL))){
            printf("Received SIGCHLD! Terminated child = %d\n",temppid);
          }

          printf("Results might be affected. Use /exit command to terminate\n");
        }else{
          waiting--;
        }

        break;
      }
    case SIGSEGV:
      printf("Received SIGSEGV! Terminating child process and exiting\n");

      //kill children process and then exit
      for(int i = 0 ; i < no_childs ; i++){
        if(pids[i] > 0) kill(pids[i],SIGKILL);
      }
      exit(1);

      break;
    default:
      break;
  }
}

//check if arguments are passed properly
int argument_check(int argc, char* argv[]){
  if(argc < 13){
    printf("Not enough arguments\n");
    return 1;
  }else if(argc > 13){
    printf("Too many arguments\n");
    return 1;
  }else{
    int iflag = 0;
    int bflag = 0;
    int mflag = 0;
    int sflag = 0;
    int tflag = 0;
    int cflag = 0;

    for(int i = 1 ; i < argc ; i += 2){
      if(!strcmp(argv[i],"-i")){
        iflag++;
        if(iflag == 2){
          printf("-i flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-s")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-m")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-b")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-c")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-t")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if((working_dir = opendir(argv[i+1])) == NULL){
          perror("file :");
          return 1;
        }
        dir_name = argv[i+1];
      }else if(!strcmp(argv[i],"-s")){
        sflag++;
        if(sflag == 2){
          printf("-s flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-i")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-m")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-b")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-c")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-t")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        for(int j = 0 ; j < strlen(argv[i+1]) ; j++){
          if(!isdigit(argv[i+1][j])){
            printf("Argument after -s flag must be a number\n");
            return 1;
          }
        }
        bloom_size = atoi(argv[i+1]);
        bloom_string = malloc(strlen(argv[i+1]) + 1);
        strcpy(bloom_string, argv[i+1]);
        if(bloom_size < 10){
          printf("Bloom size cant be less than 10. Setting bloom size to default value\n");
          bloom_size = default_bloom_size;
          free(bloom_string);
          bloom_string = malloc(strlen(default_bloom_string_size) + 1);
          strcpy(bloom_string, default_bloom_string_size);
        }
        set_bloom_size(bloom_size);
      }else if(!strcmp(argv[i], "-m")){
        mflag++;
        if(mflag == 2){
          printf("-m flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-i")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-s")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-b")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-c")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-t")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        for(int j = 0 ; j < strlen(argv[i+1]) ; j++){
          if(!isdigit(argv[i+1][j])){
            printf("Argument after -m flag must be a number\n");
            return 1;
          }
        }
        no_childs = atoi(argv[i+1]);
        if(no_childs <= 0){
          printf("Number of child processes cant be 0. Setting number of children to default value\n");
          no_childs = DEFAULT_CHILD;
        }
      }else if(!strcmp(argv[i], "-b")){
        bflag++;
        if(bflag == 2){
          printf("-b flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-i")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-s")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-m")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-c")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-t")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        for(int j = 0 ; j < strlen(argv[i+1]) ; j++){
          if(!isdigit(argv[i+1][j])){
            printf("Argument after -b flag must be a number\n");
            return 1;
          }
        }
        buffer = atoi(argv[i+1]);
        buffer_string = malloc(strlen(argv[i+1]) + 1);
        strcpy(buffer_string,argv[i+1]);
        if(buffer < sizeof(int)){
          printf("Buffer size cant be less than sizeof(int). Setting buffer size to default value 1024\n");
          buffer = DEFAULT_BUFFER;
          free(buffer_string);
          buffer_string = malloc(strlen(DEFAULT_BUFFER_STRING) + 1);
          strcpy(buffer_string,DEFAULT_BUFFER_STRING);
        }
      }else if(!strcmp(argv[i], "-c")){
        cflag++;
        if(cflag == 2){
          printf("-b flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-i")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-s")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-m")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-b")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-t")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        for(int j = 0 ; j < strlen(argv[i+1]) ; j++){
          if(!isdigit(argv[i+1][j])){
            printf("Argument after -c flag must be a number\n");
            return 1;
          }
        }
        cyclic_buffer_size = malloc(strlen(argv[i+1]) + 1);
        strcpy(cyclic_buffer_size,argv[i+1]);

        int get_size = atoi(argv[i+1]);
        if(get_size < 1){
          printf("Cyclic buffer size cant be less than 1! Setting cyclic buffer size to default value of 3\n");
          free(cyclic_buffer_size);
          cyclic_buffer_size = malloc(strlen(default_cyclic_size) + 1);
          strcpy(cyclic_buffer_size, default_cyclic_size);
        }

      }else if(!strcmp(argv[i], "-t")){
        tflag++;
        if(tflag == 2){
          printf("-b flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-i")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-s")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-m")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-b")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-c")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        for(int j = 0 ; j < strlen(argv[i+1]) ; j++){
          if(!isdigit(argv[i+1][j])){
            printf("Argument after -t flag must be a number\n");
            return 1;
          }
        }
        no_threads = malloc(strlen(argv[i+1]) + 1);
        strcpy(no_threads,argv[i+1]);

        int get_size = atoi(argv[i+1]);
        if(get_size < 1){
          printf("Number of threads cant be less than 1! Setting number of threads to default value of 3\n");
          free(no_threads);
          no_threads = malloc(strlen(default_threads) + 1);
          strcpy(no_threads, default_threads);
        }

      }
    }

    return 0;
  }
}


//reads commands from terminal
void command_line(){
  int flag = 0;

  do{
    waiting = 0;
    printf("\nPlease enter a command\n");

    char* copy = NULL;

    char line[1024];
    int len = 1024;

    //Get user input
    if((fgets(line, len, stdin)) == NULL){
      continue;
    }

    //If user input is just newline continue
    if(!strcmp(line , "\n")){
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

    //remove newline from last string
    strtok(commands[commands_size - 1], "\n");

    if(!strcmp(commands[0],"/travelRequest")){
      if(!no_of_arguments(commands_size, 6, 0)){
        if(is_integer(commands[1])){
          if(is_date(commands[2])){
            if(is_string(commands[3]) && is_string(commands[4])){
              int return_id;
              //return the process id of the process responsible from given origin country
              if((return_id = return_catalog_pid(country_responsible_pid, responsible_size,commands[3]))){
                int index = -1;
                //checks the bloom filter to see if we need to comunicate with child process
                //to confirm request.Else reject it
                if((index = pid_hash_check_bloom(records, return_id, commands[5], commands[1], commands[3], commands[2])) >= 0){
                  for(int i = 0 ; i < commands_size ; i++){
                    //we dont need to send country info
                    if(i != 3 && i != 4){
                      write(socket_fds[index], commands[i], strlen(commands[i]));
                      if(i != commands_size - 1) write(socket_fds[index], " ", strlen(" "));
                    }
                  }
                  kill(pids[index],SIGUSR2);

                  requests++;

                  waiting = 1;
                  int timeout = 5000;
                  int ret = 1;
                  int answer = 0;
                  int gflag = 0;
                  int break_flag = 0;
                  char buf[buffer];
                  char* chop = NULL;
                  char* string = NULL;

                  //wait for child to write all info
                  while(waiting){

                    global_pfds[index].fd = socket_fds[index];
                    global_pfds[index].events = 0;
                    global_pfds[index].events |= POLLIN;

                    ret = poll(global_pfds, no_childs, timeout);

                    if(ret > 0){
                      int size = 0;
                      //wait for child to write answer
                      while(waiting);

                      //read size of answer
                      read(global_pfds[index].fd, &size, sizeof(int));

                      if(size == 0) break;

                      //read info in one go
                      if(size <= buffer && size > 0){

                        char temp_buf[size];

                        if(read(global_pfds[index].fd, temp_buf, size) == -1){
                          perror("769");
                        }
                        

                        string = malloc(size + 1);
                        for(int j = 0 ; j < size ; j++){
                          string[j] = temp_buf[j];
                        }
                        string[size] = '\0';
                      }else{
                        //else construct returned info
                        int divi = size / buffer;
                        int mod = size % buffer;
                        string = malloc(size + 1);
                        
                        for(int j = 0 ; j < divi ; j++){
                          if(read(global_pfds[index].fd, buf, buffer) == -1){
                            perror("785:");
                          }
                          if(j==0){
                            
                            for(int k = 0 ; k < buffer ; k++){
                              string[k] = buf[k];
                            }
                            string[(j+1)*buffer] = '\0';
                          }else{
                            int n = 0;
                            for(int k = j*buffer ; k < (j+1)*buffer ; k++){
                              string[k] = buf[n];
                              n++;
                            }
                            string[(j+1)*buffer] = '\0';
                          }
                        }
                        if(mod > 0){

                          char temp_buf[mod];
                          
                          if(read(global_pfds[index].fd,temp_buf,mod) == -1){
                            perror("807");
                          }

                          int n = 0;
                          for(int j = divi * buffer ; j < divi * buffer + mod ; j++){
                            string[j] = temp_buf[n];
                            n++;
                          }
                          string[divi * buffer + mod] = '\0';
                          
                        }
                      }

                      char* saveptr = NULL;
                      chop = strtok_r(string," ", &saveptr);
                      //error in reading
                      if(chop == NULL){
                        printf("Error %s\n",chop);
                        gflag = -1;
                        break;
                      }
                      //if answer is YES
                      if(!strcmp(chop, "YES")){
                        
                        //take the date
                        chop = strtok_r(NULL," ",&saveptr);

                        //Again error
                        if(chop == NULL){
                          printf("Error %s\n",chop);
                          gflag = -1;
                          break;
                        }
                        //See if vaccination date is after travel date and reject
                        if(compareDate(chop, commands[2]) >= 0){
                          printf("Vaccination date cant be bigger than travel date.Error\n");
                          gflag = -1;
                          break;
                        }

                        //check if vaccination date was less than 6 months before travel date
                        if(six_months_prior(chop, commands[2])){
                          printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
                          accepted++;
                          answer = 1;
                          break;
                        }else{
                          printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
                          rejected++;
                          break;
                        }

                      }else if(!strcmp(chop, "NO")){
                        printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
                        rejected++;
                        break;
                      }else{
                        printf("Error %s\n",chop);
                        gflag = -1;
                        break;
                      }

                    }else{
                      //If poll times out two times break
                      break_flag++;
                      if(break_flag == 2) break;
                      printf("Waiting for answer\n");
                    }

                  }
                  if(string) free(string);
                  //if there was no error record new request
                  if(gflag != -1) pid_new_request(records, return_id, commands[3], commands[5], commands[2], answer);

                }else{
                  printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
                  rejected++;
                }
              }else{
                printf("Origin country does not exist\n");
              }
            }
          }else{
            printf("Third argument must be a date\n");
          }
        }else{
          printf("Second argument must be an integer\n");
        }
      }
    }else if(!strcmp(commands[0],"/travelStats")){
      if(!no_of_arguments(commands_size, 4, 5)){
        if(commands_size == 5){
          if(is_date(commands[2]) && is_date(commands[3])){
            if(compareDate(commands[2], commands[3])){
              if(is_string(commands[4])){
                int return_id;
                //return process id responsible for requested country
                if((return_id = return_catalog_pid(country_responsible_pid, responsible_size,commands[4])) > 0){
                  printf("ret id %d\n",return_id);
                  travelStats(records, commands[1], commands[2], commands[3], commands[4], return_id);
                }else{
                  printf("Country not found\n");
                }
              }else{
                printf("Fourth argument cant contain numbers\n");
              }
            }else{
              printf("First date cant be bigger than the second date\n");
            }
          }else{
            printf("Second and third argument must be a date\n");
          }
        }else{
          //else find requests for all countries
          travelStats(records, commands[1], commands[2], commands[3], NULL, -1);
        }
      }
    }else if(!strcmp(commands[0],"/addVaccinationRecords")){
      if(!no_of_arguments(commands_size, 2, 0)){
        if(is_string(commands[1])){
          int return_id;
          if((return_id = return_catalog_pid(country_responsible_pid, responsible_size,commands[1]))){
            
            //find index for global structures
            int index = pid_hash_return_index(records, return_id);
            //same process as when the processes where initializing and sending info
            //but this time only for one process

            sigset_t set2;

            sigemptyset(&set2);
            sigaddset(&set2, SIGUSR1);

            char buf[buffer];
            int ret = 1;
            int timeout = 5000;

            waiting = 1;
            kill(return_id, SIGUSR1);
            global_pfds[index].fd = socket_fds[index];
            global_pfds[index].events = POLLIN;
            while(waiting);

            while(ret > 0){
              ret = poll(global_pfds, no_childs, timeout);

              
              if(ret == -1) perror("957");
              
              if(ret > 0){


                if(global_pfds[index].revents == POLLIN){
                  int size = 0;
                  char* desease = NULL;

                  if(read(global_pfds[index].fd, &size, sizeof(int)) == -1){
                    perror("967");
                  }

                  kill(pids[index],SIGUSR1);
                  while(received == 0);
                  received = 0;

                  if(size <= buffer && size > 0){

                    char temp_buf[size];

                    if(read(global_pfds[index].fd, temp_buf, size) == -1){
                      perror("982:");
                    }

                    desease = malloc(size + 1);
                    for(int j = 0 ; j < size ; j++){
                      desease[j] = temp_buf[j];
                    }
                    desease[size] = '\0';
                  }else{
                    int divi = size / buffer;
                    int mod = size % buffer;
                    desease = malloc(size + 1);
                    
                    //printf("size = %d divi %d mod %d\n",size,divi,mod);
                    
                    for(int j = 0 ; j < divi ; j++){
                      if(read(global_pfds[index].fd, buf, buffer) == -1){
                        perror("999:");
                      }
                      if(j==0){
                        
                        for(int k = 0 ; k < buffer ; k++){
                          desease[k] = buf[k];
                        }
                        desease[(j+1)*buffer] = '\0';
                      }else{
                        int n = 0;
                        for(int k = j*buffer ; k < (j+1)*buffer ; k++){
                          desease[k] = buf[n];
                          n++;
                        }
                        desease[(j+1)*buffer] = '\0';
                      }
                    }
                    if(mod > 0){

                      char temp_buf[mod];
                      
                      if(read(global_pfds[index].fd,temp_buf,mod) == -1){
                        perror("1021");
                      }

                      int n = 0;
                      for(int j = divi * buffer ; j < divi * buffer + mod ; j++){
                        desease[j] = temp_buf[n];
                        n++;
                      }
                      desease[divi * buffer + mod] = '\0';
                      
                    }
                  }
                  kill(pids[index],SIGUSR1);
                  while(received == 0);
                  received = 0;

                  if(read(global_pfds[index].fd, &size, sizeof(int)) == -1){
                    perror("1041");
                  }
                  kill(pids[index],SIGUSR1);
                  while(received == 0);
                  received = 0;

                  int* temp_bloom = malloc(sizeof(int) * size);

                  if(size <= buffer && size <= 10000){
                    if(read(global_pfds[index].fd, temp_bloom, sizeof(int) * size) == -1){
                      perror("1054");
                    }
                  }else{
                    int div = size / buffer;
                    int mod = size % buffer;

                    for(int j = 0 ; j < div ; j++){
                      
                      if(read(global_pfds[index].fd, &temp_bloom[j*buffer], sizeof(int)*buffer) == -1){
                        perror("1063");
                      }
                      kill(pids[index], SIGUSR1);
                      
                      while(received == 0);
                      received = 0;
                    }

                    if(mod > 0){
                      if( read(global_pfds[index].fd, &temp_bloom[div], sizeof(int)*mod) == -1){
                        perror("1076");
                      }
                    }
                    
                  }

                  //update bloom filters for desease for all countries
                  pid_hash_update_bloom(records, return_id, desease, temp_bloom);                  
                  free(temp_bloom);

                  free(desease);
                  desease = NULL;


                  printf("Data read %d\n",pids[index]);
                  global_pfds[index].fd = socket_fds[index];
                  global_pfds[index].events = POLLIN;
                }
              }
            }

          }else{
            printf("Country not found\n");
          }
        }else{
          printf("Second argument cant contain numbers\n");
        }
      }
    }else if(!strcmp(commands[0],"/searchVaccinationStatus")){
      if(!no_of_arguments(commands_size, 2, 0)){
        if(is_integer(commands[1])){
          
          //send citizen id to all children to find which one is responsible for it
          for(int i = 0 ; i < no_childs ; i++){
            global_pfds[i].fd = socket_fds[i];
            global_pfds[i].events = 0;
            global_pfds[i].events |= POLLIN;

            if(write(socket_fds[i], commands[0], strlen(commands[0])) == -1){
              perror("1113");
            }
            write(socket_fds[i], " ", strlen(" "));
            write(socket_fds[i], commands[1], strlen(commands[1]));

            kill(pids[i], SIGUSR2);
          }


          int timeout = 5000;
          int ret = 1;
          char word[buffer];
          int n = 0;

          while(ret > 0){
            
            ret = poll(global_pfds, no_childs, timeout);

            if(ret == -1) perror("1132");

            if(ret > 0){

              //read all info given by children
              for(int i = 0 ; i < no_childs ; i++){
                if(global_pfds[i].revents == POLLIN){
                  if((n = read(global_pfds[i].fd, word, buffer)) > 0){
                    for(int j = 0 ; j < n ; j++){
                      printf("%c",word[j]);
                    }
                  }

                  global_pfds[i].fd = socket_fds[i];
                  global_pfds[i].events = 0;
                  global_pfds[i].events |= POLLIN;

                }                 
              }


            }

          }

        }else{
          printf("Second argument cant contain characters\n");
        }
      }
    }else if(!strcmp(commands[0],"/exit")){
      termination_flag = 1;
      printf("Exiting program\n");
      for(int i = 0 ; i < no_childs ; i++){
        if(write(socket_fds[i], commands[0], strlen(commands[0])) == -1){
          perror("1113");
        }
        waiting = 3;

        kill(pids[i], SIGUSR2);
      }
      flag = 1;
      
    }else{
      printf("Wrong command\n");
    }

    for(int i = 0 ; i < commands_size ; i++){
      free(commands[i]);
    }
    free(commands);
    free(copy);

    if(flag) break;

  }while(1);


  while(waiting);
  free_global_variables();

}

//just for the name(code is in pid_print_requests)
void travelStats(PIDHASH records, char* desease, char* start_date, char* end_date, char* country, int pid){
  if(records == NULL) return;

  pid_print_requests(records, desease, start_date, end_date, country, pid);
}

//Initializes the global variables
void initialize_global_variables(void){
  records = pid_hash_initialize();
  pids = malloc(sizeof(int) * no_childs);
  socket_fds = malloc(sizeof(int) * no_childs);
  for(int i = 0 ; i < no_childs ; i++){
    socket_fds[i] = -1;
    pids[i] = -1;
  }
}

//frees global variables and deletes temporary reading files
void free_global_variables(void){
  
  if(pids != NULL) free(pids);
  if(bloom_string != NULL) free(bloom_string);
  if(buffer_string != NULL) free(buffer_string);
  if(working_dir != NULL) closedir(working_dir);
  if(no_threads != NULL) free(no_threads);
  if(cyclic_buffer_size != NULL) free(cyclic_buffer_size);

  pid_hash_free(records);

  for(int i = 0 ; i < no_childs ; i++){
    close(socket_fds[i]);

    for(int j = 0 ; j < argument_counter[i] ; j++){
      free(argument_array[i][j]);
    }
    free(argument_array[i]);
  }

  free(argument_counter);
  free(argument_array);
  free(socket_fds);
  
  //returns all countries the program was responsible for to send to logger
  char** temp = return_catalog_countries(country_responsible_pid, responsible_size);

  //call logger and create log_file
  logger(getpid(), temp, responsible_size, requests, accepted, rejected);

  for(int i = 0 ; i < responsible_size ; i++){
    free(temp[i]);
  }

  free(temp);

  free_catalog(country_responsible_pid, responsible_size);

}