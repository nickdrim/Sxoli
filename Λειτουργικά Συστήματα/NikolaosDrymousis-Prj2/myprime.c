#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"

int main(int argc, char* argv[]){
	if(argc != 7){
		printf("Number of arguments must be 7");
		exit(1);
	}

	int noProcesses = 0;

	for(int i=0 ; i < argc ; i++){
		switch(i){
			case 0:
				if(compareString(argv[i], "./myprime")){
					printf("Name of executable is wrong\n");
					exit(1);
				}
				break;
			case 1:
				if(compareString(argv[i], "-l")){
					printf("Second argument is wrong\n");
					exit(1);
				}
				break;
			case 2:
				if(validInt(argv[i])){
					printf("Third argument must be a number\n");
					exit(1);
				}
				break;
			case 3:
				if(compareString(argv[i], "-u")){
					printf("Fourth argument is wrong\n");
					exit(1);
				}
				break;
			case 4:
				if(validInt(argv[i])){
					printf("Fifth argument must be a number\n");
					exit(1);
				}
				if(atoi(argv[i]) < 1){
					printf("Fifth argument must be greater than 0\n");
					exit(1);
				}
				if(atoi(argv[i-2]) >= atoi(argv[i])){
					printf("Lowerbound cannot be greater or equal than upperbound\n");
					exit(1);
				}
				break;
			case 5:
				if(compareString(argv[i], "-w")){
					printf("Sixth argument is wrong\n");
					exit(1);
				}
				break;
			case 6:
				if(validInt(argv[i])){
					printf("Seventh argument must be a number\n");
					exit(1);
				}

				//If needed set a limit to the process

				noProcesses = atoi(argv[6]);
				int limit_process = (atoi(argv[4]) - atoi(argv[2])) / (atoi(argv[6])*atoi(argv[6]));
				if(limit_process == 0){
					noProcesses = (int)sqrt((float)atoi(argv[4]) - atoi(argv[2]));
				}else if(noProcesses > 100){
					noProcesses = 100;
				}
				
				break;
			default:
				break;
		}
	}

	return processes(atoi(argv[2]), atoi(argv[4]), noProcesses);

}