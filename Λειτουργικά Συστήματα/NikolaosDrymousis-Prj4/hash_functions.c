#include "quic.h"

const float loadFactor = 0.6;
int defaultSize = 1000;
int size = 0;

//hash function
int hash(char* key){
  unsigned int h=0;
	int i;

	for(i=0 ; i < strlen(key) ; i++){
		h=(h<<5 | h>>27);
		h+=(unsigned int)key[i];
	}

	return h % defaultSize;
}

//Create filent struct node
filent create_filent(struct dirent* data){
	filent temp = malloc(sizeof(file_info));
	temp->ino = data->d_ino;
	temp->name = malloc(strlen(data->d_name)+1);
	strcpy(temp->name,data->d_name);
	temp->off = data->d_off;
	temp->reclen = data->d_reclen;

	return temp;
}

//free filent struct
void free_filent(filent data){
	free(data->name);
	free(data);
}

//Initialize hash table
HTable hash_initialize(){
  size = defaultSize / loadFactor;
  HTable hash = malloc(sizeof(hashtable) * size);

  for(int i=0 ; i < size ; i++){
    hash[i].start = NULL;
  }

  return hash;
}

//Insert data to hash table
void hash_insert(HTable records, struct dirent* data, char* path){
	if(records == NULL){
		printf("Hash table is uninitialized\n");
		return;
	}
	char* temp = NULL;
	temp = malloc(strlen(path) + strlen(data->d_name) + 2);
	strcpy(temp,path);
	strcat(temp,"/");
	strcat(temp,data->d_name);


	int h = hash(temp);

	if(records[h].start == NULL){
		records[h].start = new_list_node(create_filent(data), temp);
	}
	else{
		list_insert(&(records[h].start), new_list_node(create_filent(data), temp));
	}
	
	free(temp);

	return;
}

//Recursively insert the directory and its containts to hash table
void hash_directory_insert(TNode* root, HTable records, char* dest){
	DIR *dest_dir = NULL;

  if((dest_dir = opendir(dest)) == NULL){
		perror("hash opendir :");
		return;
	}

  struct dirent *dir_T;

	if(*root == NULL){
		*root = new_tree_node(dest);
	}

  while((dir_T = readdir(dest_dir)) != NULL){
		
		char* temp = malloc(strlen(dest) + strlen(dir_T->d_name) + 2);
		strcpy(temp, dest);
		strcat(temp,"/");
		strcat(temp,dir_T->d_name);
		struct stat A;
		if(stat(temp,&A) != 0){
			free(temp);
			return;
		}
		
    if(S_ISDIR(A.st_mode)){
      if (strcmp(dir_T->d_name, ".") == 0 || strcmp(dir_T->d_name, "..") == 0){
        free(temp);
				continue;
			}

			if((*root)->subfolders == NULL){
				(*root)->subfolders = malloc(sizeof(tree));
				(*root)->subfolders[0] = NULL;
				(*root)->subfolder_counter++;
			}else{
				(*root)->subfolder_counter++;
				(*root)->subfolders = realloc((*root)->subfolders,sizeof(TNode)*(*root)->subfolder_counter);
				(*root)->subfolders[(*root)->subfolder_counter - 1] = NULL;
			}

			hash_directory_insert(&((*root)->subfolders[(*root)->subfolder_counter - 1]), records, temp);
    }else{
			if((*root)->files == NULL){
				(*root)->files = malloc(sizeof(filent));
				(*root)->file_counter++;
			}else{
				(*root)->file_counter++;
				(*root)->files = realloc((*root)->files, sizeof(filent) * (*root)->file_counter);
			}
			(*root)->files[(*root)->file_counter - 1] = create_filent(dir_T);
		}

		free(temp);
		hash_insert(records, dir_T, dest);
  }
	closedir(dest_dir);
}

//If file is found or modifies change its value
void hash_search_update(HTable records, char* dest, char* name,int change){
	if(records == NULL){
		printf("Hash table is uninitialized\n");
		return;
	}

	int h = hash(name);

	if(records[h].start == NULL){
		//printf("File %s not found in target directory\n",name);
		return;
	}else{
		list_search_update(records[h].start, dest, name, change);
		return;
	}

}

//Same as hash_search_update but changes to default modified value for directories
void hash_directory_search(HTable records, char* path){
	if(records == NULL){
		printf("Hash table is uninitialized\n");
		return;
	}

	int h = hash(path);

	if(records[h].start == NULL){
		//printf("File %s not found in target directory\n",path);
		return;
	}else{
		list_directory_search(records[h].start, path);
		return;
	}
}

//Find and delete unchanged files
void hash_delete_unchanged_files(HTable records){
	if(records == NULL) return;

	for(int i = 0 ; i < size ; i++){
		if(records[i].start != NULL){
			list_delete_unchanged_files(records, records[i].start);
		}
	}
}

//change the value so they wont be changed again
void hash_search_deleted(HTable records, char* path){
	if(records == NULL) return;

	int h = hash(path);

	if(records[h].start == NULL){
		//printf("File %s not found in target directory\n",path);
		return;
	}else{
		list_search_deleted(records[h].start, path);
		return;
	}
}

//Print containts of hash table
void hash_print(HTable records){
	if(records == NULL){
		printf("empty\n");
		return;
	}

	for(int i = 0 ; i < size ; i++){
		if(records[i].start != NULL){
			printf("slot[%d] : ",i);
			list_print(records[i].start);
		}
	}
}

//Free allocated memory for hash table
void hash_free(HTable records){
	if(records == NULL){
		printf("Hash table is uninitialized\n");
		return;
	}

	for(int i = 0 ; i < size ; i++){
		if(records[i].start != NULL){
			list_free(records[i].start);
		}
	}

	free(records);
}