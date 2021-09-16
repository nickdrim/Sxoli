#include "quic.h"

int dflag = 0;
int lflag = 0;
char* origindir = NULL;
char* destdir = NULL;

int main(int argc, char* argv[]){
  initialize_global_variables();
  argument_check(argc,argv);

  //Start time of program
  clock_t begin = clock();
  
  DIR *origin = NULL;
  DIR *dest = NULL;

  //If origin directory doesnt exist
  if((origin = opendir(origindir)) == NULL){
    perror("opendir :");
    free(origindir);
    free(destdir);
    exit(1);
  }else if(errno == EACCES){
    //If we have no access rights
    perror("Access :");
    free(origindir);
    free(destdir);
    exit(1);
  }

  //If directory does not exist create it
  if((dest = opendir(destdir)) == NULL){
    if(vflag){
      printf("Directory %s doesn't exist\n",destdir);
      printf("Creating directory with path %s\n",destdir);
    }
    cp_function(origindir, destdir, lflag);

    //end time of program
    clock_t end = clock();

    //Uncomment to check hard links
    //inode_info(origin);
    //inode_info(dest);

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("there are %d files/directories in the hierarchy\n", files_seen);
    printf("number of entities copied is %d\n", file_copied);
    printf("copied %ld bytes in %f at %f bytes/sec\n",total_bytes,time_spent,total_bytes / time_spent);

    free(destdir);
    free(origindir);
    closedir(origin);
    exit(0);
  }else if(errno == EACCES){
    //if we have no access rights
    perror("Access :");
    closedir(origin);
    free(origindir);
    free(destdir);
    exit(1);
  }

  //if destination folder exists
  if(vflag) printf("Copying files from directory %s to directory %s\n",origindir,destdir);
  cp_existing_function(origindir,destdir,lflag,dflag);

  clock_t end = clock();

  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
 
  printf("there are %d files/directories in the hierarchy\n", files_seen);
  printf("number of entities copied is %d\n", file_copied);
  printf("copied %ld bytes in %f at %f bytes/sec\n",total_bytes,time_spent,total_bytes / time_spent);

  free(destdir);
  free(origindir);

  closedir(origin);
  closedir(dest);

  return 0;

}

//Check if arguments are valid
void argument_check(int argc, char* argv[]){
  if(argc < 3){
    printf("Too few arguments\n");
    exit(1);
  }
  else if(argc > 6){
    printf("Too many arguments\n");
    exit(1);
  }
  else{
    for(int i = 1; i < argc-2 ; i++){
      if(!strcmp(argv[i], "-v")){
        if(!vflag) vflag = 1;
        else{
          printf("v flag is repeated\n");
          exit(1);
        }
      }else if(!strcmp(argv[i], "-d")){
        if(!dflag) dflag = 1;
        else{
          printf("d flag is repeated\n");
          exit(1);
        }
      }else if(!strcmp(argv[i], "-l")){
        if(!lflag) lflag = 1;
        else{
          printf("l flag is repeated\n");
          exit(1);
        }
      }else{
        printf("Wrong argument order\n");
        exit(1);
      }
    }
    char* temp = NULL;
    if(argv[argc-2][0] != '/'){
      temp = malloc(strlen("/") + strlen(argv[argc-2]) + 1);
      strcpy(temp,"/");
      strcat(temp,argv[argc-2]);

    }else{
      temp = malloc(strlen(argv[argc-2]) + 1);
      strcpy(temp,argv[argc-2]);
    }
    

    char* path = getenv("HOME");
    DIR *k;
    if((k = opendir(temp))){
      origindir = malloc(strlen(temp) + 1); 
      strcpy(origindir,temp);
      closedir(k);
    }else{
      printf("Origin directory does not exist. Exiting...\n");
      free(temp);
      exit(1);
    }


    //if destination folder exists
    if((k = opendir(argv[argc-1]))){
      destdir = malloc(strlen(argv[argc-1]) + 1); 
      strcpy(destdir,argv[argc-1]);
      closedir(k);
    }else{
      //If it does not exist copy home path to the name so we can create it
      destdir = malloc(strlen(path) + strlen(argv[argc-1]) + 2);
      strcpy(destdir,path);
      strcat(destdir,"/");
      strcat(destdir,argv[argc-1]);
    }

    free(temp);

  }
}

//initializes extern global variables in quic.h
void initialize_global_variables(void){
  vflag = 0;
  file_copied = 0;
  files_seen = 0;
  total_bytes = 0;
}