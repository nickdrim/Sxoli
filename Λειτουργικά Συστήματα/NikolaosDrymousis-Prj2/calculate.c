#include "header.h"

int processes(int lowerbound, int upperbound, int noProcesses){
  
	int prevlb = lowerbound;
	if(lowerbound < 1) lowerbound = 1;
  
	int bounds[noProcesses][noProcesses+1];
	for(int i=0 ; i < noProcesses ; i++){
    for(int j=0 ; j < noProcesses + 1 ; j++){
      bounds[i][j] = 0;
    }
  }
  int step = (upperbound-lowerbound) / (noProcesses*noProcesses);
  int mod = (upperbound - lowerbound) % (noProcesses*noProcesses);
  int current = lowerbound;

  for(int i=0 ; i < noProcesses ; i++){
    for(int j=0 ; j < noProcesses + 1 ; j++){
      if(i != 0 && j == 0){
        bounds[i][j] = bounds[i-1][noProcesses];	
      }else{
        bounds[i][j] += current;
        current += step;
        if(i == noProcesses - 1 && j == noProcesses-1) current += mod;
      }
    }
  }
	
	int pipefd[noProcesses][2];
	for(int i = 0; i < noProcesses ; i++){
		if(pipe(pipefd[i]) < 0){
			if(i > 0){
				for(int j = 0; j < i  ; j++){
					close(pipefd[j][0]);
					close(pipefd[j][1]);
				}
			}
			perror("Error :");
		}
	}

	int no_signals = 0;

	pid_t first_pid;
	pid_t pid =0;
	if((pid = fork()) < 0){
		perror("Fork :");
		exit(1);
	}

	if(pid != 0){
		first_pid = getpid();
	}else{
		first_pid = getppid();
	}
	

	if(pid!=0){
		for(int i=0 ; i < noProcesses ; i++){
			if(pid==0){
				for(int k = 0 ; k < noProcesses ; k++){
					if(k!=i){
						close(pipefd[k][0]);
						close(pipefd[k][1]);
					}else{
						close(pipefd[k][0]);
					}
				}
				
				int fd_array[noProcesses];
				for(int x =0 ; x < noProcesses ; x++){
					fd_array[x] = 0;
				}
				char *filename = NULL;

				for(int j=0 ; j < noProcesses ; j++){
					char buffer[50];
					struct pollfd pfd[1];
		
					int digitsj = count_digit(j);
					int digitsi = count_digit(i);

					filename = malloc(strlen("process")+ digitsi + digitsj +1);
					strcpy(filename,"process");
					sprintf(buffer,"%d",i);
					strncat(filename,buffer,digitsi);
					sprintf(buffer,"%d",j);
					strncat(filename,buffer,digitsj);
					if(mkfifo(filename, 0777) == -1){
						if(errno != EEXIST){
							printf("Could not create fifo file\n");
							exit(1);
						}
					}

					char *prog_name = NULL;

					if(j % 3 == 0){
						prog_name = malloc(strlen("prime1")+1);
						strcpy(prog_name,"prime1");
					}else if( j % 3 == 1){
						prog_name = malloc(strlen("prime2")+1);
						strcpy(prog_name,"prime2");
					}else{
						prog_name = malloc(strlen("prime3")+1);
						strcpy(prog_name,"prime3");
					}

					char* argv2[5];
					argv2[0] = malloc(strlen(prog_name)+1);
					strcpy(argv2[0],prog_name);
					argv2[1] = malloc(strlen(filename)+1);
					strcpy(argv2[1],filename);
					sprintf(buffer,"%d",bounds[i][j]);
					argv2[2] = malloc(strlen(buffer)+1);
					strcpy(argv2[2],buffer);
					sprintf(buffer,"%d",bounds[i][j+1]-1);
					argv2[3] = malloc(strlen(buffer)+1);
					strcpy(argv2[3],buffer);
					argv2[4] = NULL;

					if((pid = fork()) < 0){
						perror("fork:");
						exit(1);
					}
					if(pid == 0){
						if(execv(prog_name, argv2) ==-1){
							perror("ERROR:");
						}
					}

					for(int k = 0 ; k < 4 ; k++){
						free(argv2[k]);
					}
					free(prog_name);

					pfd[0].fd = open(filename, O_RDONLY, O_NONBLOCK);
					fd_array[j]=pfd[0].fd;
					pfd[0].events = POLLIN;
					free(filename);

					int ready = poll(pfd, 1 , 5000);

					if(ready == -1){
						for(int k=0 ; k < j ; k++){
							close(pfd[0].fd);
						}
						exit(1);
					}else if(ready == 0){
						printf("Timeout\n");
						printf("Cannot find primes between %d and %d\n",bounds[i][j],bounds[i][j+1]-1);
						kill(pid,SIGQUIT);
						kill(getppid(),SIGUSR1);
					}else{
						if(j==noProcesses-1){
							for(int k =0 ; k < noProcesses ; k++){
								waitpid(-2,NULL,0);
							}

							for(int k = 0 ; k < noProcesses ; k++){
								int* arr = NULL;
								double *time_arr = NULL;
								int counter = 0;
								double rtime = 0;
								
								if(read(fd_array[k], &counter, sizeof(int)) < 0){
									perror("Error 1 :");
									exit(1);
								}
								
								if(counter > 0){
									arr = malloc(sizeof(int)*counter);
									time_arr = malloc(sizeof(double)*counter);

									if(read(fd_array[k], arr, sizeof(int)*counter) < 0){
										perror("Error :");
										exit(1);
									}

									if(read(fd_array[k], time_arr, sizeof(double)*counter) < 0){
										perror("Error1 :");
										exit(1);
									}
								}

								if(read(fd_array[k], &rtime, sizeof(double)) < 0){
									perror("Error :");
									exit(1);
								}
								close(fd_array[k]);

								if(write(pipefd[i][1], &counter, sizeof(int)) < 0){
									return 404;
								}

								if(counter > 0){
									listnode temp;

									for(int l=0 ; l < counter ; l++){
										listNode(&temp , arr[l],time_arr[l]);
										
										if(write(pipefd[i][1], &(temp), sizeof(listnode)) < 0){
											perror("Line 180:");
											return 3;
										}
									}
								}

								if(write(pipefd[i][1], &rtime, sizeof(double)) < 0){
									perror("Error in line 188:");
									return 3;
								}

								if(counter > 0){
									free(arr);
									free(time_arr);
								}
							}
							close(pipefd[i][1]);
						}
					}
					if(j==noProcesses-1) exit(0);
				}

			}else{
				if(i!=noProcesses - 1){
					if((pid = fork()) < 0){
						perror("fork:");
						exit(1);
					}
				}
				if(i==noProcesses-1 && getpid()==first_pid){
						for(int k =0 ; k < noProcesses ; k++){
							int stat;
							waitpid(-2,&stat,0);
							if(WIFSIGNALED(stat)){
								if(stat == SIGUSR1){
									no_signals++;
								}
							}
						}

					double rtime_arr[noProcesses*noProcesses];
					for(int k = 0 ; k < noProcesses*noProcesses ; k++){
						rtime_arr[k] = 0;
					}
					LNode head = NULL,last=NULL;
					listnode temp;
					double rtime = 0;
					int rtime_counter=0;

					for(int k=0 ; k < noProcesses ; k++){
						close(pipefd[k][1]);

						for(int x=0 ; x < noProcesses ; x++){
							int counter = 0;
							if(read(pipefd[k][0], &counter, sizeof(int)) < 0){
								perror("ERROR 2:");
								return 5;
							}

							if(counter > 0){
								for(int j=0 ; j < counter ; j++){
									if(read(pipefd[k][0], &temp, sizeof(listnode)) < 0){
										printf("ERROR 1\n");
										return 5;
									}

									listInsertLast(&head, &last, temp.number,temp.item_time);
									
								}
							}

							if(read(pipefd[k][0], &rtime, sizeof(double)) < 0){
								perror("ERROR:");
								return 4;
							}

							rtime_arr[rtime_counter] = rtime;
							rtime_counter++;


						}
						close(pipefd[k][0]);
					}

					if(prevlb < 0)
						listPrint(head,prevlb,upperbound);
					else{
						listPrint(head,lowerbound,upperbound);
					}

					find_max(rtime_arr,noProcesses*noProcesses);
					find_min(rtime_arr,noProcesses*noProcesses);
					time_of_leafs(rtime_arr,noProcesses*noProcesses);
					printf("Number of USR1 signals receaved: %d\n",no_signals);
					last = NULL;
					listFree(head);
				}
			}
		}
			
	}else{

		for(int i = 0 ; i < noProcesses ; i++){
			if(i==0) close(pipefd[i][0]);
			else{
				close(pipefd[i][0]);
				close(pipefd[i][1]);
			}
		}

		int fd_array[noProcesses];
		for(int x = 0 ; x < noProcesses ; x++){
			fd_array[x] = 0;
		}
		char *filename = NULL;

		for(int j=0 ; j < noProcesses ; j++){

			char buffer[50];
			struct pollfd pfd[1];

			int digits = count_digit(j);

			filename = malloc(strlen("process")+ digits + 2);
			strcpy(filename,"process");
			sprintf(buffer,"%d",0);
			strncat(filename,buffer,digits);
			sprintf(buffer,"%d",j);
			strncat(filename,buffer,digits);
			if(mkfifo(filename, 0777) == -1){
				if(errno != EEXIST){
					printf("Could not create fifo file\n");
					exit(1);
				}
			}

			char *prog_name = NULL;

			if(j % 3 == 0){
				prog_name = malloc(strlen("prime1")+1);
				strcpy(prog_name,"prime1");
			}else if( j % 3 == 1){
				prog_name = malloc(strlen("prime2")+1);
				strcpy(prog_name,"prime2");
			}else{
				prog_name = malloc(strlen("prime3")+1);
				strcpy(prog_name,"prime3");
			}

			char* argv2[5];
			argv2[0] = malloc(strlen(prog_name)+1);
			strcpy(argv2[0],prog_name);
			argv2[1] = malloc(strlen(filename)+1);
			strcpy(argv2[1],filename);
			sprintf(buffer,"%d",bounds[0][j]);
			argv2[2] = malloc(strlen(buffer)+1);
			strcpy(argv2[2],buffer);
			sprintf(buffer,"%d",bounds[0][j+1]-1);
			argv2[3] = malloc(strlen(buffer)+1);
			strcpy(argv2[3],buffer);
			argv2[4] = NULL;

			if((pid = fork()) < 0){
				perror("fork:");
				exit(1);
			}
			if(pid == 0){
				if(execv(prog_name, argv2) ==-1){
					perror("ERROR:");
				}
			}

			for(int k = 0 ; k < 4 ; k++){
				free(argv2[k]);
			}
			free(prog_name);

			pfd[0].fd = open(filename, O_RDONLY, O_NONBLOCK);
			fd_array[j] = pfd[0].fd;
			pfd[0].events = POLLIN;
			free(filename);

			int ready = poll(pfd, 1 , 5000);

			if(ready == -1){
				for(int k=0 ; k < j ; k++){
					close(pfd[0].fd);
				}
				exit(1);
			}else if(ready == 0){
				printf("Timeout\n");
				printf("Cannot find primes between %d and %d\n",bounds[0][j],bounds[0][j+1]-1);
				kill(pid,SIGQUIT);
				kill(getppid(),SIGUSR1);
			}else{
				if(j==noProcesses-1){
					for(int k =0 ; k < noProcesses ; k++){
						waitpid(-2,NULL,0);
					}
				
					for(int k = 0 ; k < noProcesses ; k++){
						int* arr = NULL;
						int* time_arr = NULL;
						int counter = 0;
						double rtime = 0;

						if(read(fd_array[k], &counter, sizeof(int)) < 0){
							perror("Error :");
							exit(1);
						}

						if(counter > 0){
							arr = malloc(sizeof(int)*counter);
							time_arr = malloc(sizeof(double)*counter);

							if(read(fd_array[k], arr, sizeof(int)*counter) < 0){
								perror("Error :");
								exit(1);
							}

							if(read(fd_array[k], time_arr, sizeof(double)*counter) < 0){
								perror("Error2 :");
								exit(1);
							}
						}

						if(read(fd_array[k], &rtime, sizeof(double)) < 0){
							perror("Error :");
							exit(1);
						}
						close(fd_array[k]);

						if(write(pipefd[0][1], &counter, sizeof(int)) < 0){
							perror("Line 377:");
							return 404;
						}

						if(counter > 0){
							listnode temp;

							for(int l=0 ; l < counter ; l++){
								listNode(&temp , arr[l],time_arr[l]);
								
								if(write(pipefd[0][1], &temp, sizeof(listnode)) < 0){
									printf("Line 390:");
									return 3;
								}
							}
						}

						if(write(pipefd[0][1], &rtime, sizeof(double)) < 0){
							printf("Line 398:");
							return 3;
						}
						if(counter > 0){
							free(arr);
							free(time_arr);
						}

					}	
					close(pipefd[0][1]);
					
				}
			}
		}
	}
	exit(0);
}

