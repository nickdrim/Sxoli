#include "chef.h"

//Just for explanation
char* bag[3] = {
	"tomato",
	"pepper",
	"onion"
};

//Program info
int id = 1;
char* program_name = "chef";
//Turns to 0 when all saladmakers are connected
int waiting_flag = 3;

//ids of saladmakers so that error handler can send messages if something happens
pid_t error_id1 = INITIALIZED;
pid_t error_id2 = INITIALIZED;
pid_t error_id3 = INITIALIZED;


int main(int argc, char* argv[]){
	//start time
	char* start = NULL;
	time_t now;
  time(&now);

	start = malloc(strlen(ctime(&now))+1);
	strcpy(start,ctime(&now));

	//checks if arguments are given properly
  if(argument_check(argc,argv)){
		free(start);
    exit(1);
  }

	int noSalads = atoi(argv[2]);
	int restTime = atoi(argv[4]);

	//signal handling
	struct sigaction sa = {0}, se = {0};
	sa.sa_handler = usr1_handler;
	se.sa_handler = interrupt_handler;

	pid_t pid = getpid();

	//get shared memory
	ptable block = attach_memory_block(FILENAME,sizeof(table), 0);
	if(block == NULL){
		printf("Error: could not get block\n");
		exit(1);
	}

	//Initialize block values
	block->chef_pid = pid;
	block->s1_pid = INITIALIZED;
	block->s2_pid = INITIALIZED;
	block->s3_pid = INITIALIZED;
	block->onions = 0;
	block->peppers = 0;
	block->tomatoes = 0;

	//Unlinking semaphores to be safe
	if(sem_unlink(PEPPER_SEM) < 0){
		if(errno != ENOENT){
			if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
			if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
			if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
			perror("unlink/pepper");
			detach_memory_block(block);
			destroy_memory_block(FILENAME);
			exit(1);
		}
	}
	if(sem_unlink(TOMATO_SEM) < 0){
		if(errno != ENOENT){
			if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
			if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
			if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
			perror("unlink/tomato");
			detach_memory_block(block);
			destroy_memory_block(FILENAME);
			exit(1);
		}
	}
	if(sem_unlink(ONION_SEM) < 0){
		if(errno != ENOENT){
			if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
			if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
			if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
			perror("unlink/onion");
			detach_memory_block(block);
			destroy_memory_block(FILENAME);
			exit(1);
		}
	}
	if(sem_unlink(CHEF_SEM) < 0){
		if(errno != ENOENT){
			if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
			if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
			if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
			perror("unlink/chef");
			detach_memory_block(block);
			destroy_memory_block(FILENAME);
			exit(1);
		}
	}

	//Opening semaphores
	sem_t *sem_chef = sem_open(CHEF_SEM, O_CREAT|O_EXCL, 0777, 0);
	if(sem_chef == SEM_FAILED){
		if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
		if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
		if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
		perror("sem_open/chef");
		exit(1);
	}
	
	sem_t *sem_tomato = sem_open(TOMATO_SEM, O_CREAT|O_EXCL, 0777, 0);
	if(sem_tomato == SEM_FAILED){
		if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
		if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
		if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
		perror("sem_tomato/chef");
		exit(1);
	}
	sem_t *sem_pepper = sem_open(PEPPER_SEM, O_CREAT|O_EXCL, 0777, 0);
	if(sem_pepper == SEM_FAILED){
		if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
		if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
		if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
		perror("sem_pepper/chef");
		exit(1);
	}
	sem_t *sem_onion = sem_open(ONION_SEM, O_CREAT|O_EXCL, 0777, 0);
	if(sem_onion == SEM_FAILED){
		if(block->s1_pid != INITIALIZED) kill(block->s1_pid,SIGUSR2);
		if(block->s2_pid != INITIALIZED) kill(block->s2_pid,SIGUSR2);
		if(block->s3_pid != INITIALIZED) kill(block->s3_pid,SIGUSR2);
		perror("sem_onion/chef");
		exit(1);
	}
	
	sigaction(SIGINT, &se, NULL);

	//Wait for saladmakers to connect
	while(waiting_flag){
		sigaction(SIGUSR1, &sa, NULL);
		printf("Waiting for salad makers\n");
		logger(LOG_FILE, program_name, "Waiting for salad saladmakers",id, NULL, NULL);
		sleep(2);
	};

	//Initialize error_ids
	error_id1 = block->s1_pid;
	error_id2 = block->s2_pid;
	error_id3 = block->s3_pid;

	int previous_ingredient1=-1;
 	int previous_ingredient2=-1;

	//Signal saladmakers that everybody is connected
	kill(block->s1_pid,SIGUSR1);
	kill(block->s2_pid,SIGUSR1);
	kill(block->s3_pid,SIGUSR1);

	//Each coresponds to the saladmaker with the same name and checks how many salads he made
	int onion_counter = 0;
	int tomato_counter = 0;
	int pepper_counter = 0;
	int total_counter = 0;

	//Repeat until all salads are cut
	while(noSalads){

		//Select random ingredients 0 is tomato 1 is pepper and 2 is onion
		int current_ingredient1 = rand()%3;
		int current_ingredient2 = rand()%3;
		//Get name of ingredient
		char* ingredient1 = bag[current_ingredient1];
		char* ingredient2 = bag[current_ingredient2];

		//If they are the same reselect ingredient2
		if(!compareString(ingredient1, ingredient2)){
			for(int i = 0 ; i < 3 ; i++){
				if(i != current_ingredient1){
					current_ingredient2 = i;
					ingredient2 = bag[current_ingredient2];
					break;
				}
			}
		}
		//If its not the first loop
		if(previous_ingredient1!=-1){
			//If ingredients are the same as last repetition select another ingredient2
			if(current_ingredient1 == previous_ingredient1){
				if(current_ingredient2 == previous_ingredient2){
					for(int i = 0 ; i < 3 ; i++){
						if(i != current_ingredient1 && i != current_ingredient2){
							current_ingredient2 = i;
							ingredient2 = bag[current_ingredient2];
							break;
						}
					}
				}
			}else if(current_ingredient1 == previous_ingredient2){
				if(current_ingredient2 == previous_ingredient1){
					for(int i = 0 ; i < 3 ; i++){
						if(i != current_ingredient1 && i != current_ingredient2){
							current_ingredient2 = i;
							ingredient2 = bag[current_ingredient2];
							break;
						}
					}
				}
			}
		}
		//Set ingredients for next repetition
		previous_ingredient1 = current_ingredient1;
		previous_ingredient2 = current_ingredient2;

		printf("Selecting ingredients [%s] [%s]\n",ingredient1,ingredient2);
		logger(LOG_FILE, program_name, "Waiting for salad saladmakers",id, ingredient1, ingredient2);

		//Check ingredients and notify the right saladmaker
		if(!compareString(ingredient1,"tomato")){
			if(!compareString(ingredient2,"pepper")){
				//Notify saladmaker for onions
				printf("Notify saladmaker3\n");
				logger(LOG_FILE, program_name, "Notify saladmaker3",id, NULL, NULL);
				block->tomatoes++;
				block->peppers++;
				//printf("%d , %d , %d\n",block->onions,block->peppers,block->tomatoes); //uncomment to see block current values

				sem_post(sem_onion);
				sem_wait(sem_chef);
				if(noSalads < 3){
					//This saladmaker wont be called again so tell him to stop.
					kill(block->s3_pid,SIGUSR1);
					sem_post(sem_onion);
				}
				onion_counter++;

			}else{
				//Notify saladmaker for pepper
				printf("Notify saladmaker2\n");
				logger(LOG_FILE, program_name, "Notify saladmaker2",id, NULL, NULL);
				block->tomatoes++;
				block->onions++;
				//printf("%d , %d , %d\n",block->onions,block->peppers,block->tomatoes); //uncomment to see block current values
				
				sem_post(sem_pepper);
				sem_wait(sem_chef);
				if(noSalads < 3){
					kill(block->s2_pid,SIGUSR1);
					sem_post(sem_pepper);
				}
				pepper_counter++;

			}
		}else if(!compareString(ingredient1,"pepper")){
			if(!compareString(ingredient2,"tomato")){
				//Notify saladmaker for onion
				printf("Notify saladmaker3\n");
				logger(LOG_FILE, program_name, "Notify saladmaker3",id, NULL, NULL);
				block->tomatoes++;
				block->peppers++;
				//printf("%d , %d , %d\n",block->onions,block->peppers,block->tomatoes); //uncomment to see block current values

				sem_post(sem_onion);
				sem_wait(sem_chef);
				if(noSalads < 3){
					kill(block->s3_pid,SIGUSR1);
					sem_post(sem_onion);
				}
				onion_counter++;

			}else{
				//Notify saladmaker for tomato
				printf("Notify saladmaker1\n");
				logger(LOG_FILE, program_name, "Notify saladmaker1",id, NULL, NULL);
				block->onions++;
				block->peppers++;
				//printf("%d , %d , %d\n",block->onions,block->peppers,block->tomatoes); //uncomment to see block current values

				sem_post(sem_tomato);
				sem_wait(sem_chef);
				if(noSalads < 3){
					kill(block->s1_pid,SIGUSR1);
					sem_post(sem_tomato);
				}
				tomato_counter++;

			}
		}else{
			if(!compareString(ingredient2,"tomato")){
				//Notify saladmaker for pepper
				printf("Notify saladmaker2\n");
				logger(LOG_FILE, program_name, "Notify saladmaker2",id, NULL, NULL);
				block->tomatoes++;
				block->onions++;
				//printf("%d , %d , %d\n",block->onions,block->peppers,block->tomatoes); //uncomment to see block current values
				
				sem_post(sem_pepper);
				sem_wait(sem_chef);
				if(noSalads < 3){
					kill(block->s2_pid,SIGUSR1);
					sem_post(sem_pepper);
				}
				pepper_counter++;

			}else{
				//Notify saladmaker for tomato
				printf("Notify saladmaker1\n");
				logger(LOG_FILE, program_name, "Notify saladmaker1",id, NULL, NULL);
				block->onions++;
				block->peppers++;
				//printf("%d , %d , %d\n",block->onions,block->peppers,block->tomatoes); //uncomment to see block current values
				
				sem_post(sem_tomato);
				sem_wait(sem_chef);
				if(noSalads < 3){
					kill(block->s1_pid,SIGUSR1);
					sem_post(sem_tomato);
				}
				tomato_counter++;
			}
		}
		total_counter++;

		noSalads--;
		//If all salads are made notify saladmakers to stop
		if(!noSalads){
			kill(block->s1_pid,SIGUSR1);
			sem_post(sem_tomato);
			kill(block->s2_pid,SIGUSR1);
			sem_post(sem_tomato);
			kill(block->s3_pid,SIGUSR1);
			sem_post(sem_tomato);
		}

		printf("Man time for resting\n");
		logger(LOG_FILE, program_name, "Man time for resting",id, NULL, NULL);
		sleep(restTime);
	}

	//Close semaphores
	sem_close(sem_chef);
	sem_close(sem_tomato);
	sem_close(sem_onion);
	sem_close(sem_pepper);

	printf("Total #salads: %d\n", total_counter);
	printf("#salads of saladmaker1 %d : %d\n",block->s1_pid, tomato_counter);
	printf("#salads of saladmaker2 %d : %d\n",block->s2_pid, pepper_counter);
	printf("#salads of saladmaker3 %d : %d\n",block->s3_pid, onion_counter);

	detach_memory_block(block);
	destroy_memory_block(FILENAME);

	logger(LOG_FILE, program_name, NULL, id, start, NULL);
	free(start);

}

//Signal handler for receaving messages from saladmakers
void usr1_handler(int sig){
	printf("Received USR1 signal\n");
	waiting_flag--;
	return;
}

//Signal handler in case user interupts program
void interrupt_handler(int sig){
	printf("User Interrupted chef\n");
	if(error_id1 != INITIALIZED) kill(error_id1,SIGINT);
	if(error_id2 != INITIALIZED) kill(error_id2,SIGINT);
	if(error_id3 != INITIALIZED) kill(error_id3,SIGINT);
	exit(1);
}


int argument_check(int argc, char* argv[]){
  if(argc != 5){
		printf("Number of arguments must be 5\n");
		return 1;
	}

  for(int i=0 ; i < argc ; i++){
		switch(i){
			case 0:
				if(compareString(argv[i], "./chef")){
					printf("Name of executable is wrong\n");
					return 1;
				}
				break;
			case 1:
				if(compareString(argv[i], "-n")){
					printf("Second argument is wrong\n");
					return 1;
				}
				break;
			case 2:
				if(validInt(argv[i])){
					printf("Third argument must be a number\n");
					return 1;
				}
        if(atoi(argv[i]) < 1){
					printf("Fifth argument must be greater than 0\n");
					return 1;
				}
				break;
			case 3:
				if(compareString(argv[i], "-m")){
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
				break;
			default:
				break;
		}
	}

  return 0;
}