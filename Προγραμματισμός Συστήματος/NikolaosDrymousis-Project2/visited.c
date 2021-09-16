#include "header.h"

int dsize = 0;

//Creates a directory hash table
DHash dir_hash_initialize(void){
  dsize = 100;
  DHash temp = malloc(sizeof(dir_hash) * dsize);

  for(int i = 0 ; i < dsize ; i++){
    temp[i].start = NULL;
  }

  return temp;
}

//Creates and return a directory node
dir_ptr new_dir_node(char* name){

  dir_ptr temp = malloc(sizeof(directory));

  temp->dirname = malloc(strlen(name) + 1);
  strcpy(temp->dirname,name);

  char* token = strtok(name,"/");
  char* str;
  while(token){
    str = token;
    token = strtok(NULL,"/");
  }

  temp->country = malloc(strlen(str) + 1);
  strcpy(temp->country, str);

  temp->filenames = NULL;
  temp->next = NULL;
  temp->no_files = 0;

  return temp;

}

//Inserts a new directory hash table
void dir_hash_insert(DHash directories, char* name){
  if(directories == NULL) return;

  int h = hash(name, dsize);

  dir_hash_list_insert(&(directories[h].start), name);
}

//Inserts a new directory to the list of the hash table
void dir_hash_list_insert(dir_ptr* head, char* name){
  if(*head == NULL){
    *head = new_dir_node(name);
    return;
  }

  dir_ptr traverse = *head;

  while(traverse->next){
    traverse = traverse->next;
  }

  traverse->next = new_dir_node(name);
}

//Searches the directory node(dname) in the hashtable and adds a file name
//to it.If file name exists returns 0 and does not insert
int dir_hash_search_insert(DHash directories, char* dname, char* filname){
  if(directories == NULL) return 0;

  int h = hash(dname, dsize);

  return dir_hash_list_search_insert(directories[h].start, dname, filname);
}

//Searches the directory node(dname) in the list of the hashtable and adds a file name
//to it.If file name exists returns 0 and does not insert
int dir_hash_list_search_insert(dir_ptr head, char* dname, char* filename){
  if(head == NULL) return 0;
  //maybe more code if head == null

  dir_ptr traverse = head;

  while(traverse){
    if(!strcmp(traverse->dirname,dname)){
      if(traverse->filenames == NULL){
        traverse->no_files++;
        traverse->filenames = malloc(sizeof(char *) * traverse->no_files);
        traverse->filenames[0] = malloc(strlen(filename) + 1);
        strcpy(traverse->filenames[0],filename);
        return 1;
      }else{
        for(int i = 0 ; i < traverse->no_files ; i++){
          if(!strcmp(traverse->filenames[i], filename)){
            //exists
            return 0;
          }
        }

        traverse->no_files++;

        traverse->filenames = realloc(traverse->filenames, sizeof(char*) * traverse->no_files);
        traverse->filenames[traverse->no_files - 1] = malloc(strlen(filename) + 1);
        strcpy(traverse->filenames[traverse->no_files - 1], filename);

        return 1;
      }
    }

    traverse = traverse->next;
  }

  //dname doesnt exist
  return 0;
  
}

//Returns all the countries the proccess is responsible for
//to send to logger function(USED IN CASE OF SIGINT or SIGQUIT)
char** dir_hash_return_countries(DHash directories, int *size){
  if(directories == NULL) return NULL;

  char** countries = NULL;

  for(int i = 0 ; i < dsize ; i++){
    if(directories[i].start != NULL){
      dir_ptr traverse = directories[i].start;

      while(traverse){
        (*size)++;
        if(countries == NULL){
          countries = malloc(sizeof(char *) * (*size));
          countries[0] = malloc(strlen(traverse->country) + 1);
          strcpy(countries[0], traverse->country);
        }else{
          countries = realloc(countries, sizeof(char*) * (*size));
          countries[(*size) - 1] = malloc(strlen(traverse->country) + 1);
          strcpy(countries[(*size) - 1], traverse->country);
        }

        traverse = traverse->next;
      }
    }
  }

  return countries;
}

//If parent process sends a signal that there are new data in the directories, that the process
//is responsible for, search the directories to locate and insert new files and their data
int dir_hash_update_directories(DHash directories, HTable records, CHTable citizen_records){
  if(directories == NULL) return 0;

  int check = 0;

  for(int i = 0 ; i < dsize ; i++){
    if(directories[i].start){

      dir_ptr traverse = directories[i].start;

      while(traverse){
        
        DIR* temp = opendir(traverse->dirname);

        if(temp != NULL){
          struct stat F;
          struct dirent *dir = NULL;
          int fd = -1;
          char* mmap_file = NULL;

          while((dir = readdir(temp)) != NULL){
            if(!strcmp(dir->d_name, "."))
              continue;
            if(!strcmp(dir->d_name, ".."))    
              continue;
            
            char* path = malloc(strlen(traverse->dirname) + strlen(dir->d_name) + 2);
            strcpy(path,traverse->dirname);
            strcat(path,"/");
            strcat(path,dir->d_name);

            stat(path,&F);

            //if new file
            if(dir_hash_search_insert(directories,traverse->dirname,dir->d_name) == 0){
              free(path);
              continue;
            }

            check = 1;

            if((fd = open(path, O_RDONLY)) == -1){
              perror("file :");
              continue;
            }

            if((mmap_file = mmap(NULL, F.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
              perror("line 18 : mmap ");
              exit(1);
            } 

            //Parse file
            parser(records, citizen_records ,mmap_file, F.st_size);

            munmap(mmap_file, F.st_size);
            //hash_print(records);

            free(path);
          }

        }

        closedir(temp);

        traverse = traverse->next;
      }

    }
  }


  return check;
}

//Frees directory hash table
void dir_hash_free(DHash directories){
  if(directories == NULL) return;

  for(int i = 0 ; i < dsize ; i++){
    if(directories[i].start != NULL) dir_hash_list_free(directories[i].start);
  }
  
  free(directories);
}

//Frees directory list and node containts
void dir_hash_list_free(dir_ptr head){
  if(head == NULL) return;

  dir_hash_list_free(head->next);
  free(head->dirname);
  free(head->filenames);
  free(head);

}