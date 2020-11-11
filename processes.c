#include "header.h"

void processes(int lowerbound, int upperbound, int noProcesses){
  

  int bounds[noProcesses][noProcesses+1];
  int pipefd[noProcesses];
  int step = (upperbound-lowerbound) / (noProcesses*noProcesses);
  int mod = (upperbound - lowerbound) / (noProcesses*noProcesses);
  int current = lowerbound;

  for(int i=0 ; i < noProcesses ; i++){
    for(int j=0 ; j < noProcesses + 1 ; i++){
      if(i != 0 && j == 0){
        bounds[i][j] = bounds[i-1][noProcesses];
      }else{
        bounds[i][j] += current;
        current += step;
        if(i == noProcesses - 1 && j == noProcesses-1) current += mod;
      }
    }
  }

  pid_t pid = fork();
	if(pid!=0){
		for(int i=1 ; i < noProcesses+1 ; i++){
			if(pid==0){
				for(int j=0 ; j < noProcesses ; j++){
					pid = fork();
					if(pid == 0){
						//call exec("program",bounds[i][j],bounds[i][j+1])
						exit(1);
					}
					
					if(j==3) while(wait(NULL)!=-1);
				}
			}else{
				pid = fork();
			}
			if(i==3) while(wait(NULL)!=-1);
		}
		while(wait(NULL)!=-1);

		
	}else{
		for(int j=0 ; j < noProcesses ; j++){
			pid = fork();
			if(pid == 0){
				//call exec("program",bounds[0][j],bounds[0][j+1])
				exit(1);
			}
			
			if(j==3) while(wait(NULL)!=-1);
		}
		while(wait(NULL)!=-1);
	}

	while(wait(NULL)!=-1);
}

