#include "quic.h"

//If destination directory exists
void cp_existing_function(char *origin, char* dest, int lflag, int dflag){

  TNode dest_root = NULL;
  TNode origin_root = NULL;
  //Initialize structs
  HTable dest_records = hash_initialize();
  hash_directory_insert(&dest_root, dest_records, dest);
  HTable origin_records = hash_initialize();
  hash_directory_insert(&origin_root, origin_records,origin);

  //UNCOMMENT FOR INFORMATION ON THE STRUCTS
  //inorder_print(origin_root);
  //inorder_print(dest_root);
  //hash_print(origin_records);
  //hash_print(dest_records);

  compare_directories(origin_root, dest_root, dest_records, origin, dest, lflag);

  //if -d
  if(dflag){
    if(vflag) printf("\n\nDeleting files that are not in directory %s\n\n",origin);
    hash_delete_unchanged_files(dest_records);
  }

  //Uncomment to check hard links
  //inode_info(origin);
  //inode_info(dest);

  tree_free(dest_root);
  tree_free(origin_root);
  hash_free(origin_records);
  hash_free(dest_records);

}

//Functions that copies to a non existing directory
void cp_function(char *origin, char* dest, int lflag){
  char* temp = malloc(strlen(dest) + 2);
  strcpy(temp,"/");
  char* argument = NULL;

  //construct new directory
  char* token = strtok_r(dest, "/", &argument);
  while(token != NULL){
    strcat(temp,"/");
    strcat(temp,token);
    if(mkdir(temp, 0777) == -1){
      if(errno != EEXIST){
        perror("mkdir ");
        exit(1);
      }   
    }
    token = strtok_r(NULL, "/", &argument);
  }

  if(vflag) printf("directory %s created\n",temp);
  //recursively copy origin contents to destination directory
  search_directory(origin, temp, lflag);
  free(temp);

}

//Recursive function that copies files and directories to dest
void search_directory(char* origin,char* dest, int lflag){
  DIR *origin_dir = NULL;

  origin_dir = opendir(origin);

  struct dirent *dir_A;

  while((dir_A = readdir(origin_dir)) != NULL){
    
      if (strcmp(dir_A->d_name, ".") == 0 || strcmp(dir_A->d_name, "..") == 0)
        continue;

      char* new_folder = malloc(strlen(dest) + strlen(dir_A->d_name) + 2);
      strcpy(new_folder, dest);
      strcat(new_folder,"/");
      strcat(new_folder,dir_A->d_name);

      char* permition = malloc(strlen(origin) + strlen(dir_A->d_name) + 2);
      strcpy(permition, origin);
      strcat(permition, "/");
      strcat(permition, dir_A->d_name);
      
      struct stat A;
      stat(permition, &A);

    if(S_ISDIR(A.st_mode)){
      if(mkdir(new_folder, A.st_mode) == -1){
        if(errno != EEXIST){
          free(new_folder);
          free(permition);
          perror("mkdir ");
          exit(1);
        }
      }else if(vflag){
        printf("Created directory %s inside directory %s\n",new_folder,dest);
      }

      search_directory(permition,new_folder,lflag);
      total_bytes += A.st_size;
      file_copied++;
      files_seen++;
    }else{
      filent temp = create_filent(dir_A);
      create_and_copy(temp, origin, dest, lflag);
      file_copied++;
      files_seen++;
      total_bytes += A.st_size;
      free(temp->name);
      free(temp);
    }

    free(new_folder);
    free(permition);

  }
  closedir(origin_dir);
}

//Function that copies new files in an existing destination directory
void create_and_copy(filent dir_A, char* origin, char* dest, int lflag){
  char* file_open = malloc(strlen(origin) + strlen(dir_A->name) + 2);
  strcpy(file_open, origin);
  strcat(file_open,"/");
  strcat(file_open,dir_A->name);

  char* new_file = malloc(strlen(dest) + strlen(dir_A->name) + 2);
  strcpy(new_file, dest);
  strcat(new_file,"/");
  strcat(new_file,dir_A->name);
  
  struct stat A;
  lstat(file_open, &A);
  
  if(lflag){
    if(S_ISLNK(A.st_mode)){
      if(symlink(file_open, new_file) < 0){
        perror("symlink ");
      }else if(vflag){
        printf("Copied symbolic link %s at directory %s\n",new_file,dest);
        total_bytes += A.st_size;
      }
    }else{
      if(link(file_open, new_file) < 0){
        perror("link :");
      }else{
        printf("Copied hard link %s at directory %s\n",new_file,dest);
        total_bytes += A.st_size;
      }
    }

  }else{
    int file_A , file_T;
    int n = 0;
    char buf[BUFFSIZE];

    if((file_A = open(file_open, O_RDONLY)) < 0){
      perror("line 143 open :");
      exit(1);
    }

    if((file_T = open(new_file, O_WRONLY|O_CREAT|O_APPEND, A.st_mode)) < 0){
      perror("line 148 new file open :");
      printf("%s\n",new_file);
      exit(1);
    }

    while((n = read(file_A , buf , sizeof(buf))) > 0)
      write(file_T ,buf ,n);
    
    if(vflag) printf("Copied file %s at directory %s\n",new_file,dest);
    total_bytes += A.st_size;

    close(file_A);
    close(file_T);
  }

  free(file_open);
  free(new_file);
}

//Delete file/directory that does not exist in the origin directory
void delete_directory(HTable records, char* path){
  if(path == NULL) return;

  DIR* path_dir = opendir(path);
  struct dirent* dir_T;

  while((dir_T = readdir(path_dir)) != NULL){
    if (strcmp(dir_T->d_name, ".") == 0 || strcmp(dir_T->d_name, "..") == 0)
			continue;

    char* temp = malloc(strlen(path) + strlen(dir_T->d_name) + 2);
		strcpy(temp, path);
		strcat(temp,"/");
		strcat(temp,dir_T->d_name);
		struct stat T;

    lstat(temp,&T);

    if(S_ISDIR(T.st_mode)){
      delete_directory(records, temp);
      if(vflag) printf("Directory %s deleted from directory %s\n",temp, path);
    }else{
      if(vflag) printf("File %s deleted from directory %s\n",temp, path);
    }
    
    if(S_ISLNK(T.st_mode)){
      unlink(temp);
    }
    remove(temp);
    files_seen++;
    
    hash_search_deleted(records, temp);

    free(temp);
  }
  closedir(path_dir);
}

//Information function mostly to show that hard link flag works.
void inode_info(char* name){
  printf("\nCurrent directory = %s\n",name);

  struct stat D;
  DIR *d;
  struct dirent *dir;
  d = opendir(name);
  if (d)
  {
      while ((dir = readdir(d)) != NULL)
      {
        if(dir->d_type == DT_DIR){
          if (dir->d_name[0] == '.' || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
              continue;

          char* new_directory = malloc(strlen(name) + strlen(dir->d_name) + 2);
          strcpy(new_directory, name);
          strcat(new_directory,"/");
          strcat(new_directory,dir->d_name);

          inode_info(new_directory);
          
          char* token = strrchr(name,'/');
          char* temp;
          token = strtok_r(token,"/",&temp);
          free(new_directory);

          

          printf("\nReturned to directory = %s\n",token);
        }else{

          char* new_file = malloc(strlen(name) + strlen(dir->d_name) + 2);
          strcpy(new_file, name);
          strcat(new_file,"/");
          strcat(new_file,dir->d_name);
          printf("\n%s\n", dir->d_name);
          stat(new_file, &D);


          printf("ID of device %ld\n", (long) D.st_dev);
          printf("Inode number is %ld\n", (long) D.st_ino);
          printf("Number of hard links %ld\n", (long) D.st_nlink);
          printf("Size %ld\n", (long) D.st_size);
          printf("Size in blocks %ld\n", (long) D.st_blksize);
          printf("Inode number is %ld\n", (long) D.st_blocks);
          printf("Last modified %s\n",  ctime(&D.st_mtim.tv_sec));
          printf("\n\n");
          free(new_file);
        }
          
      }
      closedir(d);
  }
}
