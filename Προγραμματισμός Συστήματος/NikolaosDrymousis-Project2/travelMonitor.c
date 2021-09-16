#include "header.h"

//GLOBAL VARIABLES
//They are global because the might be needed in multiple
//functions and the signal handler in case of signal
//from child processes or SIGINT etc

DIR* working_dir = NULL;
char* file = NULL;
char* dir_name = NULL;

//bloom filter info
int bloom_size = 0;
char* bloom_string = NULL; //pass as argument to exec

//number of child process
int no_childs = 0;

//Buffer size info
int buffer = 0;
char* buffer_string = NULL; //pass as argument to exec

//Array to store the ids of child processes
int* pids = NULL;

//Array to store the ids of the original child process(in case one terminated)
int* terminated_pids = NULL;

//Arrays for holding the names and file descriptors of read and write fifos of processes
char** write_fifos = NULL;
int* write_fds = NULL;
char** read_fifos = NULL;
int* read_fds = NULL;

//Flag so that if parent gets SIGINT signal the execution of the last command continues
int current_command = 0;

//flag that indicates if the process must terminate after executing last command
int sigint_flag = 0;

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

//flag that is 1 if /exit command is given.
int termination_flag = 0;

//Signal set for sigwait
sigset_t set;
int sig;
int *sigptr = &sig;

struct pollfd* global_pfds;

//declarations of local functions
void free_global_variables(void);
void initialize_global_variables(void);

int main(int argc, char *argv[]){
  //checks if arguments are correct
  if(argument_check(argc, argv)){
    printf("Run command is as follows:\n");
    printf("./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir");
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
  waiting = no_childs;

  char buf[buffer];

  for(int i = 0 ; i < no_childs ; i++){

    //Create read and write fifo file for specific child
    sprintf(buf, "%d", i+1);
    write_fifos[i] = malloc(strlen("writefifo") + strlen(buf) + 1);
    strcpy(write_fifos[i], "writefifo");
    strcat(write_fifos[i], buf);

    sprintf(buf, "%d", i+1);
    read_fifos[i] = malloc(strlen("readfifo") + strlen(buf) + 1);
    strcpy(read_fifos[i], "readfifo");
    strcat(read_fifos[i], buf);

    if(mkfifo(write_fifos[i], 0777) == -1){
		  if(errno != EEXIST){
			  printf("Could not create fifo file\n");
			  exit(1);
		  }
	  }

    if(mkfifo(read_fifos[i], 0777) == -1){
		  if(errno != EEXIST){
			  printf("Could not create fifo file\n");
			  exit(1);
		  }
	  }

    //Open read fifo
    if((read_fds[i] = open(read_fifos[i], O_CREAT|O_RDONLY|O_NONBLOCK, 0777)) == -1){
      perror("line 78:");
    }

    //Initialize pollfd struct
    pfds[i].fd = read_fds[i];
    pfds[i].events = 0;
    pfds[i].events |= POLLIN;

    int tempid = fork();

    if(tempid == 0){
      if(execl("./Monitor","./Monitor",write_fifos[i], read_fifos[i], bloom_string, buffer_string, "1", "0", (char *)NULL) == -1){
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
      pids[i] = tempid;
      terminated_pids[i] = tempid;
      pid_hash_insert(records, tempid, write_fifos[i], read_fifos[i], i);
    }

    //open write fifo file
    if((write_fds[i] = open(write_fifos[i], O_CREAT | O_WRONLY|O_NONBLOCK, 0777)) == -1){
      perror("line 82:");
      exit(1);
    }

  }

  //So that they can be accessed from everywhere in the program
  global_pfds = pfds;
  

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
      country_responsible_pid[responsible_size-1]->pid = pids[rrobin];
    }else{
      country_responsible_pid = realloc(country_responsible_pid,sizeof(catalog) * responsible_size);
      country_responsible_pid[responsible_size-1] = malloc(sizeof(country_catalog));
      country_responsible_pid[responsible_size-1]->country = malloc(strlen(dir->d_name) + 1);
      strcpy(country_responsible_pid[responsible_size-1]->country, dir->d_name);
      country_responsible_pid[responsible_size-1]->pid = pids[rrobin];
    }

    //Write to proccess that is responsible for country
    write(write_fds[rrobin], fullpath, strlen(fullpath));
    write(write_fds[rrobin], "\n", strlen("\n"));

    //associate hash table with country and directories
    pid_hash_search_insert(records, pids[rrobin], fullpath, dir->d_name);

    rrobin++;
  

    free(fullpath);

  }

  printf("Sending signals to children\n");

  //Inform children parent has finished writting info to fifos
  for(int i = 0 ; i < no_childs ; i++){
    kill(pids[i], SIGUSR1);
  }

  //Initialize new set to accept only one signal


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
            if(read(read_fds[i], temp_bloom, sizeof(int) * size) == -1){
              perror("251");
            }
          }else{
            //else calculate how many reads it will take
            int div = size / buffer;
            int mod = size % buffer;

            for(int j = 0 ; j < div ; j++){
              //continue reading from pointer
              if(read(read_fds[i], &temp_bloom[j*buffer], sizeof(int)*buffer) == -1){
                perror("273");
              }
              //inform and wait
              kill(pids[i], SIGUSR1);
              
              while(received == 0);
              received = 0;
              
            }

            if(mod > 0){
              if( read(read_fds[i], &temp_bloom[div], sizeof(int)*mod) == -1){
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
          global_pfds[i].fd = read_fds[i];
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
      //raise termination flag
      termination_flag = 1;
      
      //if parent was in the middle of executing a command return
      if(current_command){
        //raise flag so that the parent will know after execution of last command to terminate
        sigint_flag = 1;
        break;
      }

      //close file descriptors, delete files and kill children processes
      for(int i = 0 ; i < no_childs ; i++){
        close(write_fds[i]);
        close(read_fds[i]);
        unlink(write_fifos[i]);
        unlink(read_fifos[i]);
        free(write_fifos[i]);
        free(read_fifos[i]);

        kill(pids[i],SIGKILL);
      }

      free_global_variables();

      exit(1);
      break;
    case SIGCHLD:

      //if a child terminated and parent is not ready to exit execution
      //reinitialize child process.Else ignore SIGCHLD to terminate program
      if(termination_flag == 0){
        int temppid = -1;
        int wstatus;

        //find the id of the terminated child
        if((temppid = wait(&wstatus))){
          printf("terminated child = %d\n",temppid);

          if(WIFSIGNALED(wstatus)){
            printf("%d\n",WTERMSIG(wstatus));
          }

          //return the fifos associated with previous process
          char* rfile = pid_hash_return_read_fifo(records, temppid);
          char* wfile = pid_hash_return_write_fifo(records, temppid);

          char temp_buf[1024];
          //find the index of the global variables that info of the terminated
          //process is stored
          int temp_index = pid_hash_return_index(records, temppid);
          
          //convert the first proccess that was terminated id into a string
          //to pass as argument.This is in case there have been multiple terminations
          //of processes responsible for same countries.So that all the next processes
          //read the file of the first process and reinitialize the structures
          sprintf(temp_buf, "%d", terminated_pids[temp_index]);

          int new_id = fork();

          if(new_id == 0){
            if(execl("./Monitor","./Monitor", wfile, rfile, buffer_string, buffer_string, "0", temp_buf, (char *)NULL) == -1){
              perror("exec :");
              exit(1);
            }
          }else{
            sigwait(&set, sigptr);

            //change all the structures associated with previous id to new process id
            change_catalog_pid(country_responsible_pid, responsible_size, temppid, new_id);
            int index = pid_hash_update(records, temppid, new_id);
            pids[index] = new_id;
          }
        }
      }

      break;
    case SIGSEGV:
      printf("Segmentation fault\n");

      //delete files and kill children process and then exit
      for(int i = 0 ; i < no_childs ; i++){
        close(write_fds[i]);
        close(read_fds[i]);
        unlink(write_fifos[i]);
        unlink(read_fifos[i]);
        kill(pids[i],SIGKILL);
      }
      exit(1);

      break;
    default:
      break;
  }
}

//check if arguments are passed properly
int argument_check(int argc, char* argv[]){
  if(argc < 9){
    printf("Not enough arguments\n");
    return 1;
  }else if(argc > 9){
    printf("Too many arguments\n");
    return 1;
  }else{
    int iflag = 0;
    int bflag = 0;
    int mflag = 0;
    int sflag = 0;

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
        for(int j = 0 ; j < strlen(argv[i+1]) ; j++){
          if(!isdigit(argv[i+1][j])){
            printf("Argument after -b flag must be a number\n");
            return 1;
          }
        }
        buffer = atoi(argv[i+1]);
        buffer_string = malloc(strlen(argv[i+1]) + 1);
        strcpy(buffer_string,argv[i+1]);
        if(buffer < 1){
          printf("Buffer size cant be less than 1. Setting buffer size to default value 1024\n");
          buffer = DEFAULT_BUFFER;
          free(buffer_string);
          buffer_string = malloc(strlen(DEFAULT_BUFFER_STRING) + 1);
          strcpy(buffer_string,DEFAULT_BUFFER_STRING);
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
      current_command = 1;
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
                      write(write_fds[index], commands[i], strlen(commands[i]));
                      if(i != commands_size - 1) write(write_fds[index], " ", strlen(" "));
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

                    global_pfds[index].fd = read_fds[index];
                    global_pfds[index].events = 0;
                    global_pfds[index].events |= POLLIN;

                    ret = poll(global_pfds, no_childs, timeout);

                    if(ret > 0){
                      int size = 0;
                      //read size of answer
                      read(global_pfds[index].fd, &size, sizeof(int));

                      

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
      current_command = 1;
      if(!no_of_arguments(commands_size, 4, 5)){
        if(commands_size == 5){
          if(is_date(commands[2]) && is_date(commands[3])){
            if(compareDate(commands[2], commands[3])){
              if(is_string(commands[4])){
                int return_id;
                //return process id responsible for requested country
                if((return_id = return_catalog_pid(country_responsible_pid, responsible_size,commands[4]))){
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
      current_command = 1;
      if(!no_of_arguments(commands_size, 2, 0)){
        if(is_string(commands[1])){
          int return_id;
          if((return_id = return_catalog_pid(country_responsible_pid, responsible_size,commands[1]))){
            
            //find index for global structures
            int index = pid_hash_return_index(records, return_id);

            //same process as when the processes where initializing and sending info
            //but this time only for one process

            char buf[buffer];
            int ret = 1;
            int timeout = 2000;

            waiting = 1;
            kill(return_id, SIGUSR1);
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
                    if(read(read_fds[index], temp_bloom, sizeof(int) * size) == -1){
                      perror("1054");
                    }
                  }else{
                    int div = size / buffer;
                    int mod = size % buffer;

                    for(int j = 0 ; j < div ; j++){
                      
                      if(read(read_fds[index], &temp_bloom[j*buffer], sizeof(int)*buffer) == -1){
                        perror("1063");
                      }
                      kill(pids[index], SIGUSR1);
                      
                      while(received == 0);
                      received = 0;
                    }

                    if(mod > 0){
                      if( read(read_fds[index], &temp_bloom[div], sizeof(int)*mod) == -1){
                        perror("1076");
                      }
                    }
                    
                  }

                  //update bloom filters for desease for all countries
                  pid_hash_update_bloom(records, return_id, desease, temp_bloom);                  
                  free(temp_bloom);

                  free(desease);
                  desease = NULL;

                  global_pfds[index].fd = read_fds[index];
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
      current_command = 1;
      if(!no_of_arguments(commands_size, 2, 0)){
        if(is_integer(commands[1])){
          
          //send citizen id to all children to find which one is responsible for it
          for(int i = 0 ; i < no_childs ; i++){
            global_pfds[i].fd = read_fds[i];
            global_pfds[i].events = 0;
            global_pfds[i].events |= POLLIN;

            if(write(write_fds[i], commands[0], strlen(commands[0])) == -1){
              perror("1113");
            }
            write(write_fds[i], " ", strlen(" "));
            write(write_fds[i], commands[1], strlen(commands[1]));

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

                  global_pfds[i].fd = read_fds[i];
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
      printf("Exiting program\n");
      //to ignore SIGCHLD
      termination_flag = 1;
      flag = 1; 
    }else{
      printf("Wrong command\n");
    }

    for(int i = 0 ; i < commands_size ; i++){
      free(commands[i]);
    }
    free(commands);
    free(copy);
    //free(line);

    current_command = 0;

    //if a sigint was sent during execution of a command
    //when it finishes resent signal
    if(sigint_flag){
      kill(getpid(), SIGINT);
    }


    if(flag) break;

  }while(1);

  for(int i = 0 ; i < no_childs ; i++){
    kill(pids[i], SIGKILL);
    
    close(write_fds[i]);
    close(read_fds[i]);

    unlink(write_fifos[i]);
    unlink(read_fifos[i]);
    
    free(write_fifos[i]);
    free(read_fifos[i]);
    
  }

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
  terminated_pids = malloc(sizeof(int) * no_childs);
  for(int i = 0 ; i < no_childs ; i++){
    pids[i] = -1;
    terminated_pids[i] = -1;
  }
  write_fifos = malloc(sizeof(char*) * no_childs);
  write_fds = malloc(sizeof(int) * no_childs);
  read_fifos = malloc(sizeof(char*) * no_childs);
  read_fds = malloc(sizeof(int) * no_childs);
}

//frees global variables and deletes temporary reading files
void free_global_variables(void){
  
  if(pids != NULL) free(pids);
  for(int i = 0 ; i < no_childs ; i++){
    char buf[100];

    sprintf(buf, "%d", terminated_pids[i]);
    char* temp = malloc(strlen(buf) + strlen(".txt") + 1);
    strcpy(temp, buf);
    strcat(temp, ".txt");
    unlink(temp);
    free(temp);
    
  }

  if(terminated_pids != NULL) free(terminated_pids);
  if(write_fifos != NULL) free(write_fifos);
  if(read_fifos != NULL) free(read_fifos);
  if(write_fds != NULL) free(write_fds);
  if(read_fds != NULL) free(read_fds);
  if(bloom_string != NULL) free(bloom_string);
  if(buffer_string != NULL) free(buffer_string);
  if(working_dir != NULL) closedir(working_dir);
  pid_hash_free(records);
  
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