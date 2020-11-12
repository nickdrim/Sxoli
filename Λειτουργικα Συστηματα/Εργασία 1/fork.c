#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(){

	pid_t pid;

	for(int i=0 ; i < 3 ; i++){
		pid = fork();
	}
}