#include "quic.h"

//Create new node for tree
TNode new_tree_node(char* data){
	TNode temp = malloc(sizeof(tree));
	temp->folder = malloc(strlen(data) + 1);
	strcpy(temp->folder, data);
	temp->subfolders = NULL;
	temp->files = NULL;
	temp->file_counter = 0;
	temp->subfolder_counter = 0;

	return temp;
}

//Recursive function that traverses the tree nodes and compare their contents
void compare_directories(TNode origin_root, TNode dest_root, HTable dest_records, char* origin, char* dest, int lflag){
	if(origin_root == NULL || dest_root == NULL){
		return;
	}

	//Compare all files in the node
	for(int i = 0 ; i < origin_root->file_counter ; i++){
		filent origin_temp = origin_root->files[i];
		int flag = 1;
    files_seen++;

		for(int j = 0 ; j < dest_root->file_counter ; j++){
			filent dest_temp = dest_root->files[j];

			//if files have the same name
			if(strcmp(origin_temp->name, dest_temp->name) == 0){
				struct stat A, T;
				flag = 0;

				char* origin_filename = malloc(strlen(origin) + strlen(origin_temp->name) + 2);
				strcpy(origin_filename, origin);
				strcat(origin_filename, "/");
				strcat(origin_filename, origin_temp->name);
				lstat(origin_filename, &A);

				char* dest_filename = malloc(strlen(dest) + strlen(dest_temp->name) + 2);
				strcpy(dest_filename, dest);
				strcat(dest_filename, "/");
				strcat(dest_filename, dest_temp->name);
				lstat(dest_filename, &T);

				//if files have the same size
				if(A.st_size == T.st_size){
					
					//If last modified time of files is not the same
					if(strcmp(ctime(&A.st_mtim.tv_sec),ctime(&T.st_mtim.tv_sec))){
						remove(dest_filename);
						if(lflag){
              if(S_ISLNK(A.st_mode)){
                if(symlink(origin_filename,dest_filename) != -1){
                  hash_search_update(dest_records, dest, dest_filename, 1);
                  if(vflag) printf("Symbolick link %s updated\n",dest_filename);
                  file_copied++;
                  total_bytes += A.st_size;
                }else{
                  perror("Cant link files : ");
                  exit(1);
                }
              }else{
                if(link(origin_filename,dest_filename) != -1){
                  hash_search_update(dest_records, dest, dest_filename, 1);
                  if(vflag) printf("Hard link %s updated\n",dest_filename);
                  total_bytes += A.st_size;
                  file_copied++;
                }else{
                  perror("Cant link files : ");
                  exit(1);
                }
              }	
						}else{
							int file_A , file_T;
							int n = 0;
							char buf[BUFFSIZE];

							if((file_A = open(origin_filename, O_RDONLY)) < 0){
								perror("open :");
								exit(1);
							}

							if((file_T = open(dest_filename, O_WRONLY|O_CREAT|O_APPEND, A.st_mode)) < 0){
								perror("new file open ");
								exit(1);
							}

							while((n = read(file_A , buf , sizeof(buf))) > 0)
								write(file_T ,buf ,n);

              if(vflag) printf("File %s updated\n",dest_filename);
              total_bytes += A.st_size;
              file_copied++;
							
							close(file_A);
							close(file_T);

							hash_search_update(dest_records, dest, dest_filename, 1);
						}
					}else{
						//If lflag is up and files are not hardlinked together update them
						if(lflag){
							if(A.st_ino == T.st_ino){
								hash_search_update(dest_records, dest, dest_filename, 0);
								printf("File %s already exists in directory %s\n",dest_filename, dest);
							}else{
								if(S_ISLNK(A.st_mode)){
									remove(dest_filename);
									if(symlink(origin_filename,dest_filename) != -1){
										hash_search_update(dest_records, dest, dest_filename, 1);
                    if(vflag) printf("Symbolick link %s updated\n",dest_filename);
                    total_bytes += A.st_size;
                    file_copied++;
									}else{
										perror("Cant link files :");
									}
								}else{
									remove(dest_filename);
									if(link(origin_filename,dest_filename) != -1){
										hash_search_update(dest_records, dest, dest_filename, 1);
                    if(vflag) printf("Hard link %s updated\n",dest_filename);		
                    total_bytes += A.st_size;
                    file_copied++;
									}else{
										perror("Cant link files :");
									}
								}
							}
						}else{
							//Files are the same
							hash_search_update(dest_records, dest, dest_filename, 0);
							if(vflag) printf("File %s already exists in directory %s\n",dest_filename, dest);
						}

					}
				}else{
					//Size of file has changed so we need to copy it again
          remove(dest_filename);
          if(lflag){
            if(S_ISLNK(A.st_mode)){
              if(symlink(origin_filename,dest_filename) != -1){
                hash_search_update(dest_records, dest, dest_filename, 1);
                if(vflag) printf("Symbolick link %s updated\n",dest_filename);
                file_copied++;
                total_bytes += A.st_size;
              }else{
                perror("Cant link files : ");
                exit(1);
              }
            }else{
              if(link(origin_filename,dest_filename) != -1){
                hash_search_update(dest_records, dest, dest_filename, 1);
                if(vflag) printf("Hard link %s updated\n",dest_filename);
                total_bytes += A.st_size;
                file_copied++;
              }else{
                perror("Cant link files : ");
                exit(1);
              }
            }	
          }else{
            int file_A , file_T;
            int n = 0;
            char buf[BUFFSIZE];

            if((file_A = open(origin_filename, O_RDONLY)) < 0){
              perror("open :");
              exit(1);
            }

            if((file_T = open(dest_filename, O_WRONLY|O_CREAT|O_APPEND, A.st_mode)) < 0){
              perror("new file open ");
              exit(1);
            }

            while((n = read(file_A , buf , sizeof(buf))) > 0)
              write(file_T ,buf ,n);

            
            if(vflag) printf("File %s updated\n",dest_filename);

            total_bytes += A.st_size;
            file_copied++;
            
            close(file_A);
            close(file_T);

            hash_search_update(dest_records, dest, dest_filename, 1);
          }
        }
				free(origin_filename);
				free(dest_filename);
				break;
			}
		}
		
		if(flag){
			//file not found in directory
      if(vflag) printf("File %s does not exit in directory %s\n",origin_temp->name,dest);
			create_and_copy(origin_temp, origin, dest, lflag);
		}
	}

	//Search all subdirectories of node
	for(int i = 0 ; i < origin_root->subfolder_counter ; i++){
		char* origin_temp = origin_root->subfolders[i]->folder;
		int flag = 1;

		char* origin_folder = malloc(strlen(origin_root->subfolders[i]->folder) + 1);
		strcpy(origin_folder,origin_root->subfolders[i]->folder);
		char* chop1 = origin_folder;
		char* argument1;

		char* token1 = strtok_r(chop1,"/",&argument1);
		while(token1 != NULL){
			token1 = strtok_r(NULL,"/",&argument1);
			if(token1 != NULL) chop1 = token1;
		}
		
		for(int j = 0 ; j < dest_root->subfolder_counter ; j++){
			char* dest_temp = dest_root->subfolders[j]->folder;

			char* dest_folder = malloc(strlen(dest_root->subfolders[j]->folder) + 1);
			strcpy(dest_folder,dest_root->subfolders[j]->folder);
			char* chop2 = dest_folder;
			char* argument2;

			char* token2 = strtok_r(chop2,"/",&argument2);
			while(token2 != NULL){
				token2 = strtok_r(NULL,"/",&argument2);
				if(token2 != NULL) chop2 = token2;
			}
			
			if(strcmp(chop1, chop2) == 0){
        if(vflag) printf("Directory %s exists\n",chop1);
				flag = 0;

				hash_directory_search(dest_records, dest_temp);
				
				//if they have the same name use the function to compare their files
				compare_directories(origin_root->subfolders[i], dest_root->subfolders[j], dest_records, origin_temp, dest_temp, lflag);
        files_seen++;	
				free(dest_folder);
				break;
			}
			
			free(dest_folder);
		}
		
		if(flag){
			//Directory from origin node does not exist in dest so we must copy its containts
			if (vflag) printf("Directory %s does not exist in %s\n",chop1,dest);
			cp_function(origin,dest,lflag);
		}

		free(origin_folder);
	}

}

//Usefull function to see the containts of trees
void inorder_print(TNode root){
	if(root == NULL) return;

	printf("directory = %s\n",root->folder);
	printf("no sub = %d and no fil = %d\n",root->subfolder_counter,root->file_counter);
	printf("files in directory\n");
	for(int i = 0 ; i < root->file_counter ; i++){
		printf("file = %s\n",root->files[i]->name);
	}

	for(int i = 0 ; i < root->subfolder_counter ; i++){
		inorder_print(root->subfolders[i]);
	}
}

//Free allocated space
void tree_free(TNode root){
	if(root == NULL) return;

	for(int i = 0 ; i < root->subfolder_counter ; i++){
		tree_free(root->subfolders[i]);
	}
	for(int i = 0 ; i < root->file_counter ; i++){
		free_filent(root->files[i]);
	}
	if(root->subfolder_counter) free(root->subfolders);
	if(root->file_counter) free(root->files);
	free(root->folder);
	free(root);
}