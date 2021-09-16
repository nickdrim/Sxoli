#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "help.h"

typedef struct table{
  int tomatoes;
  int peppers;
  int onions;
  pid_t chef_pid, s1_pid, s2_pid, s3_pid;
}table, *ptable;


ptable attach_memory_block(char* filename, int size, int key);
int detach_memory_block(ptable block);
int destroy_memory_block(char* filename);
void logger(char* log_file, char* program_name, char* message, int no, char* item1, char* item2);

#define IPC_RESULT_ERROR -1
#define CHEF_SEM "/chefsem"
#define TOMATO_SEM "/tomato"
#define ONION_SEM "/onion"
#define PEPPER_SEM "/pepper"
#define INITIALIZED -1
#define FILENAME "chef.c"
#define LOG_FILE "chef.txt"

#endif