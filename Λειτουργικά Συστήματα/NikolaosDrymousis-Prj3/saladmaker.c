#include "saladmaker.h"

//Waiting for chef to notify salad maker that everyone is connected
int waiting_flag = 1;

//If user interrupts chef proccess this flag is raised and the program ends
int error_flag = 0;

//Program info
int id = -1;
char* filename = NULL;
char* program_name = NULL;

int main(int argc, char* argv[]){
	//start time
	char* start=NULL;
	time_t now;
  time(&now);

	start = malloc(strlen(ctime(&now))+1);
	strcpy(start,ctime(&now));

  if(argument_check(argc,argv)){
		free(start);
    exit(1);
  }

	int lb = atoi(argv[2]);
	int ub = atoi(argv[4]);

	//Initialize bag of saladmaker
  ing_ptr bag = malloc(sizeof(ingredients));
	sem_t *sem_pepper = NULL;
	sem_t *sem_tomato = NULL;
	sem_t *sem_onion = NULL;

	//Depending on the ninth argument we set the bag values
  if(compareString(argv[8], "tomato")){
    if(compareString(argv[8], "onion")){
			//Set bag for pepper saladmaker
			//Program info initialization
			id = 3;
			filename = malloc(strlen("pepper_log.txt")+1);
			strcpy(filename,"pepper_log.txt");
			program_name = malloc(strlen("saladmaker2")+1);
			strcpy(program_name,"saladmaker2");
      bag->bag_ing = malloc(strlen("/pepper")+1);
      strcpy(bag->bag_ing, "/pepper");

			//Only pepper is always AVAILABLE
      bag->pepper = AVAILABLE;
      bag->onion = UNAVAILABLE;
      bag->tomato = UNAVAILABLE;

			sem_pepper = sem_open(bag->bag_ing, 0);
			if(sem_pepper == SEM_FAILED){
				perror("sem_open/pepper");
				exit(1);
			}
    }else{
			//Set bag for onion saladmaker
			//Program info initialization
			id = 4;
			filename = malloc(strlen("onion_log.txt")+1);
			strcpy(filename,"onion_log.txt");
			program_name = malloc(strlen("saladmaker3")+1);
			strcpy(program_name,"saladmaker3");
      bag->bag_ing = malloc(strlen("/onion")+1);
      strcpy(bag->bag_ing, "/onion");

			//Only Onion is always available
      bag->onion = AVAILABLE;
      bag->tomato = UNAVAILABLE;
      bag->pepper = UNAVAILABLE;

			sem_onion = sem_open(bag->bag_ing, 0);
			if(sem_onion == SEM_FAILED){
				perror("sem_open/onion");
				exit(1);
			}
    }
  }else{
		//Set bag for tomato saladmaker
		//Program info initialization
		id = 2;
		filename = malloc(strlen("tomato_log.txt")+1);
		strcpy(filename,"tomato_log.txt");
		program_name = malloc(strlen("saladmaker1")+1);
		strcpy(program_name,"saladmaker1");
    bag->bag_ing = malloc(strlen("/tomato")+1);
    strcpy(bag->bag_ing, "/tomato");

		//Only tomato is always available
    bag->tomato = AVAILABLE;
    bag->pepper = UNAVAILABLE;
    bag->onion = UNAVAILABLE;

		sem_tomato = sem_open(bag->bag_ing, 0);
		if(sem_tomato == SEM_FAILED){
			perror("sem_open/tomato");
			exit(1);
		}
  }

	sem_t *sem_chef = sem_open(CHEF_SEM,0);
	if(sem_chef == SEM_FAILED){
		perror("sem_open/chef");
		exit(1);
	}

  pid_t pid = getpid();

	//get shared memory
	ptable block = attach_memory_block(NULL,sizeof(table), atoi(argv[6]));
  if(block == NULL){
		printf("Error: could not get block\n");
		sem_close(sem_chef);
		sem_close(sem_tomato);
		sem_close(sem_onion);
		sem_close(sem_pepper);
		exit(1);
	}
  
	//Check if saladmaker with a certain ingredient already exists
  if(!compareString("/tomato",bag->bag_ing)){
		if(block->s1_pid != INITIALIZED){
			printf("Salad maker with tomato ingredient already exists\n");
			detach_memory_block(block);
			sem_close(sem_chef);
			sem_close(sem_tomato);
			free(start);
			free(program_name);
			free(filename);
			free(bag->bag_ing);
			free(bag);
			exit(1);
		}
    block->s1_pid = pid;
  }else if(!compareString("/pepper",bag->bag_ing)){
		if(block->s2_pid != INITIALIZED){
			printf("Salad maker with pepper ingredient already exists\n");
			detach_memory_block(block);
			sem_close(sem_chef);
			sem_close(sem_pepper);
			free(start);
			free(program_name);
			free(filename);
			free(bag->bag_ing);
			free(bag);
			exit(1);
		}
    block->s2_pid = pid;
  }else{
		if(block->s3_pid != INITIALIZED){
			printf("Salad maker with onion ingredient already exists\n");
			detach_memory_block(block);
			sem_close(sem_chef);
			sem_close(sem_onion);
			free(start);
			free(program_name);
			free(filename);
			free(bag->bag_ing);
			free(bag);
			exit(1);
		}
    block->s3_pid = pid;
  }
  
	//Signal handling
	struct sigaction sa = {0} , se = {0};
	sa.sa_handler = usr1_handler;
	se.sa_handler = error_handler;
  kill(block->chef_pid,SIGUSR1);

	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &se, NULL);

	//Wait till you get signal from chef that every saladmaker is connected
	while(waiting_flag){
		printf("Waiting for other saladmakers\n");
		logger(filename, program_name, "Waiting for other saladmakers",id, NULL, NULL);
		logger(LOG_FILE, program_name, "Waiting for other saladmakers",id, NULL, NULL);
		sleep(2);
	}

	//If this flag is raised than chef was terminated by a SIGINT
	if(error_flag){
		printf("Something went wrong with chef!Exiting\n");
		if(!compareString(bag->bag_ing,"/tomato")){
			sem_close(sem_tomato);
		}else if(!compareString(bag->bag_ing,"/pepper")){
			sem_close(sem_pepper);
		}else{
			sem_close(sem_onion);
		}
		detach_memory_block(block);
		sem_close(sem_chef);
		sem_close(sem_onion);
		free(start);
		free(program_name);
		free(filename);
		free(bag->bag_ing);
		free(bag);
		exit(1);
	}
	waiting_flag++;

	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &se, NULL);

	//Do this loop while there are salads to make
	while(1){
		if(!waiting_flag) break;
		printf("Waiting for ingredients\n");
		logger(filename, program_name, "Waiting for ingredients",id, NULL, NULL);
		logger(LOG_FILE, program_name, "Waiting for ingredients",id, NULL, NULL);

		//If its tomato saladmaker
		if(!compareString(bag->bag_ing,"/tomato")){
			sem_wait(sem_tomato);
			if(!waiting_flag) break;
			printf("Get ingredients\n");
			logger(filename, program_name, "Get ingredients",id, NULL, NULL);
			logger(LOG_FILE, program_name, "Get ingredients",id, NULL, NULL);

			//Take ingredients from (table) shared memory and decrement table's values
			bag->onion = block->onions;
			bag->pepper = block->peppers;
			block->onions--;
			block->peppers--;
			//printf("%d , %d , %d\n",bag->onion,bag->pepper,bag->tomato);	//Uncomment to see saladmaker's current Available ingredients
			printf("Start making salad\n");

			logger(filename, program_name, "Start making salad",id, NULL, NULL);
			logger(LOG_FILE, program_name, "Start making salad",id, NULL, NULL);
			
			//Choose a random number between lb and ub for saladmaker to work
			sleep((rand() % (ub - lb + 1)) + lb);
			if(bag->onion == AVAILABLE && bag->pepper == AVAILABLE && bag->tomato == AVAILABLE){
				printf("End making salad\n");
				logger(filename, program_name, "End making salad",id, NULL, NULL);
				logger(LOG_FILE, program_name, "End making salad",id, NULL, NULL);
			}else{
				printf("Salad failed\n");
			}
			sem_post(sem_chef);

			//If its pepper saladmaker
		}else if(!compareString(bag->bag_ing,"/pepper")){
			sem_wait(sem_pepper);
			if(!waiting_flag) break;
			printf("Get ingredients\n");
			logger(filename, program_name, "Get ingredients",id, NULL, NULL);
			logger(LOG_FILE, program_name, "Get ingredients",id, NULL, NULL);

			//Take ingredients from (table) shared memory and decrement table's values
			bag->onion = block->onions;
			bag->tomato = block->tomatoes;
			block->onions--;
			block->tomatoes--;
			//printf("%d , %d , %d\n",bag->onion,bag->pepper,bag->tomato); //Uncomment to see saladmaker's current Available ingredients
			printf("Start making salad\n");

			logger(filename, program_name, "Start making salad",id, NULL, NULL);
			logger(LOG_FILE, program_name, "Start making salad",id, NULL, NULL);

			//Choose a random number between lb and ub for saladmaker to work
			sleep((rand() % (ub - lb + 1)) + lb);
			if(bag->onion == AVAILABLE && bag->pepper == AVAILABLE && bag->tomato == AVAILABLE){
				printf("End making salad\n");
				logger(filename, program_name, "End making salad",id, NULL, NULL);
				logger(LOG_FILE, program_name, "End making salad",id, NULL, NULL);
			}else{
				printf("Salad failed\n");
			}
			sem_post(sem_chef);

			//If its onion saladmaker
		}else{
			sem_wait(sem_onion);
			if(!waiting_flag) break;
			printf("Get ingredients\n");
			logger(filename, program_name, "Get ingredients",id, NULL, NULL);
			logger(LOG_FILE, program_name, "Get ingredients",id, NULL, NULL);

			//Take ingredients from (table) shared memory and decrement table's values
			bag->tomato = block->tomatoes;
			bag->pepper = block->peppers;
			block->tomatoes--;
			block->peppers--;
			//printf("%d , %d , %d\n",bag->onion,bag->pepper,bag->tomato); //Uncomment to see saladmaker's current Available ingredients
			printf("Start making salad\n");

			logger(filename, program_name, "Start making salad",id, NULL, NULL);
			logger(LOG_FILE, program_name, "Start making salad",id, NULL, NULL);

			//Choose a random number between lb and ub for saladmaker to work
			sleep((rand() % (ub - lb + 1)) + lb);
			if(bag->onion == AVAILABLE && bag->pepper == AVAILABLE && bag->tomato == AVAILABLE){
				printf("End making salad\n");
				logger(filename, program_name, "End making salad",id, NULL, NULL);
				logger(LOG_FILE, program_name, "End making salad",id, NULL, NULL);
			}else{
				printf("Salad failed\n");
			}
			sem_post(sem_chef);
		}
		if(!waiting_flag) break;
	}

	//If this flag is raised chef was terminated with a SIGINT
	if(error_flag){
		printf("Something went wrong with chef!Exiting\n");
		if(!compareString(bag->bag_ing,"/tomato")){
			sem_close(sem_tomato);
		}else if(!compareString(bag->bag_ing,"/pepper")){
			sem_close(sem_pepper);
		}else{
			sem_close(sem_onion);
		}
		detach_memory_block(block);
		sem_close(sem_chef);
		sem_close(sem_onion);
		free(start);
		free(program_name);
		free(filename);
		free(bag->bag_ing);
		free(bag);
		exit(1);
	}

	//Closing semaphores
	sem_close(sem_chef);
	if(!compareString(bag->bag_ing,"/tomato")){
		sem_close(sem_tomato);
		block->s1_pid = id;
	}else if(!compareString(bag->bag_ing,"/pepper")){
		sem_close(sem_pepper);
		block->s2_pid = id;
	}else{
		sem_close(sem_onion);
		block->s3_pid = id;
	}
	detach_memory_block(block);

	free(bag->bag_ing);
	free(bag);

	logger(filename, program_name, NULL, id, start, NULL);
	logger(LOG_FILE, program_name, NULL, id, start, NULL);

	free(start);
	free(program_name);
	free(filename);
	
}

//Signal handler for receaving messages from chef
void usr1_handler(int sig){
	printf("Received USR1 signal\n");
	waiting_flag--;
	return;
}

//Signal handler in case user interupts chef program
void error_handler(int sig){
	printf("Received USR2 signal\n");
	waiting_flag--;
	error_flag++;
	return;
}


int argument_check(int argc, char* argv[]){
  if(argc != 9){
		printf("Number of arguments must be 9\n");
		return 1;
	}

  for(int i=0 ; i < argc ; i++){
		switch(i){
			case 0:
				if(compareString(argv[i], "./saladmaker")){
					printf("Name of executable is wrong\n");
					return 1;
				}
				break;
			case 1:
				if(compareString(argv[i], "-t1")){
					printf("Second argument is wrong\n");
					return 1;
				}
				break;
			case 2:
				if(validInt(argv[i])){
					printf("Third argument must be a number\n");
					return 1;
				}
        if(atoi(argv[i]) < 0){
					printf("Third argument must be greater or equal to 0\n");
					return 1;
				}
				break;
			case 3:
				if(compareString(argv[i], "-t2")){
					printf("Fourth argument is wrong\n");
					return 1;
				}
				break;
			case 4:
				if(validInt(argv[i])){
					printf("Fifth argument must be a number\n");
					return 1;
				}
				if(atoi(argv[i]) < 0){
					printf("Fifth argument must be greater or equal to 0\n");
					return 1;
				}
        if(atoi(argv[i-2]) >= atoi(argv[i])){
          printf("Fifth argument must be greater than third argume\n");
          return 1;
        }
				break;
      case 5:
        if(compareString(argv[i], "-s")){
					printf("Sixth argument is wrong\n");
					return 1;
				}
				break;
      case 6:
				if(validInt(argv[i])){
					printf("Seventh argument must be a number\n");
					return 1;
				}
				if(atoi(argv[i]) < 0){
					printf("Seventh argument must be greater than 0\n");
					return 1;
				}
				break;
      case 7:
        if(compareString(argv[i], "-i")){
					printf("Eighth argument is wrong\n");
					return 1;
				}
        break;
      case 8:
        if(compareString(argv[i], "tomato")){
					if(compareString(argv[i], "onion")){
            if(compareString(argv[i], "pepper")){
              printf("Ninth argument is wrong.Ingredient doesnt exist\n");
              return 1;
            }
          }
				}
        break;
			default:
				break;
		}
	}

  return 0;
}