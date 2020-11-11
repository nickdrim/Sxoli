#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "help.h"

int main(int argc, char* argv[]){
	if(argc != 7){
		printf("Number of arguments must be 7");
		exit(1);
	}

	for(int i=0 ; i < argv ; i++){
		switch(i){
			case 0:
				if(!compareString(argv[i], "./myprime")){
					printf("Name of executable is wrong\n");
					exit(1);
				}
				break;
			case 1:
				if(!compareString(argv[i], "-l")){
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
				if(!compareString(argv[i], "-u")){
					printf("Fourth argument is wrong\n");
					exit(1);
				}
				break;
			case 4:
				if(validInt(argv[i])){
					printf("Fifth argument must be a number\n");
					exit(1);
				}
				break;
			case 5:
				if(!compareString(argv[i], "-w")){
					printf("Sixth argument is wrong\n");
					exit(1);
				}
				break;
			case 6:
				if(validInt(argv[i])){
					printf("Seventh argument must be a number\n");
					exit(1);
				}
				break;
			default:
				break;
		}
	}

	processes(atoi(argv[2]), atoi(argv[4]), atoi(argv[6]));

}