#include "shared_memory.h"

static int get_shared_block(char *filename, int size){
  key_t key;

  key = ftok(filename, 0);
  if(key == IPC_RESULT_ERROR){
    return IPC_RESULT_ERROR;
  }

  return shmget(key, size, 0777 | IPC_CREAT);
}

ptable attach_memory_block(char* filename, int size, int key){
  int shared_block_id; 
  
  if(filename != NULL){
    shared_block_id = get_shared_block(filename, size);
    printf("ID for shared memory is %d\n",shared_block_id);
  }
  else shared_block_id = key;
  
  if(shared_block_id == IPC_RESULT_ERROR){
    return NULL;
  }

  ptable result = shmat(shared_block_id, NULL, 0);
  if(result == (ptable)IPC_RESULT_ERROR){
    return NULL;
  }

  return result;
}

int detach_memory_block(ptable block){
  return (shmdt(block) != IPC_RESULT_ERROR);
}

int destroy_memory_block(char* filename){
  int shared_block_id = get_shared_block(filename, 0);

  if (shared_block_id == IPC_RESULT_ERROR){
    return 0;
  }

  return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}

//Function for log file
void logger(char* log_file, char* program_name, char* message, int no, char* item1, char* item2){
  FILE *fp = NULL;
  if((fp = fopen(log_file,"a")) == NULL){
    perror("LOG_FILE");
    return;
  }
  //Message time
  time_t now;
  time(&now);

  if(item1 == NULL){
    //Add message to log
    fprintf(fp, "[%.24s][%d][%s][%s]\n", ctime(&now), no, program_name, message);
  }else if(message != NULL){
    //Add a specific massage to log. Only chef uses it
    fprintf(fp, "[%.24s][%d][%s][%s %s %s]\n", ctime(&now), no, program_name, message, item1, item2);
  }else{
    //List of time spaces
    fprintf(fp, "[%.24s][%d][%s][%.24s - %.24s]\n", ctime(&now), no, program_name, item1, ctime(&now));
  }

  fclose (fp);
  return;
}