#include "quic.h"

//Create a new list node
LNode new_list_node(filent item, char* path){
	LNode temp = malloc(sizeof(list));
	temp->info = item;
	temp->changed = -1;
	temp->next = NULL;
	temp->fullpath = malloc(strlen(path) + 1);
	strcpy(temp->fullpath,path);

	return temp;
}

//Insert node to list
void list_insert(LNode* head, LNode data){
	if(*head == NULL){
		*head = data;
		return;
	}

	data->next = *head;
	*head = data;
}

//Update change value of dest item in list
void list_search_update(LNode head, char* dest, char* name, int change){
	if(head == NULL){
		printf("List is empty");
		return;
	}

	LNode traverse = head;

	while(traverse != NULL){
		char* path = malloc(strlen(dest) + strlen(traverse->info->name) + 2);
		strcpy(path,dest);
		strcat(path,"/");
		strcat(path,traverse->info->name);

		if(strcmp(path,name) == 0){
			
			struct stat A,T;
			stat(path, &T);
			stat(name,&A);

			if(S_ISDIR(A.st_mode)){
				//If one is file and the otherdirectory dont change
				if(S_ISDIR(T.st_mode)){
					//printf("File %s exists in target location\n",name);
					traverse->changed = change;
					free(path);
					return;
				}
			}else if(S_ISREG(T.st_mode)){
				//printf("File %s exists in target location\n",name);
				free(path);
				traverse->changed = change;
				return;
			}
		}
		free(path);
		traverse = traverse->next;
	}

	//printf("File %s not found in target directory\n",name);
	return;
}

//Same as list_search_update
void list_directory_search(LNode head, char* path){
	if(head == NULL) return;

	LNode traverse = head;

	while(traverse != NULL){

		if(strcmp(traverse->fullpath, path) == 0){
			struct stat A,T;
			stat(path, &T);
			stat(traverse->fullpath,&A);

			if(S_ISDIR(A.st_mode)){

				if(S_ISDIR(T.st_mode)){
					//printf("File %s exists in target location\n",name);
					traverse->changed = 1;
					return;
				}
			}else if(S_ISREG(T.st_mode)){
				//printf("File %s exists in target location\n",name);
				traverse->changed = 1;
				return;
			}
			
		}

		traverse = traverse->next;
	}
}

//change the value so they wont be changed again
void list_search_deleted(LNode head, char* path){
	if(head == NULL) return;

	LNode traverse = head;

	while(traverse != NULL){

		if(strcmp(path,traverse->fullpath) == 0){
			
			struct stat A,T;
			if(stat(path, &T) != 1){
				return;
			}
			if(stat(traverse->fullpath,&A) != 1){
				return;
			}

			if(S_ISDIR(A.st_mode)){

				if(S_ISDIR(T.st_mode)){
					//printf("File %s exists in target location\n",name);
					traverse->changed = 1;
					return;
				}
			}else if(S_ISREG(T.st_mode)){
				//printf("File %s exists in target location\n",name);
				traverse->changed = 1;
				return;
			}
			
			//code
		}
		traverse = traverse->next;
	}

	return;
}

//Find file in list and call delete function
void list_delete_unchanged_files(HTable records, LNode head){
	if(head == NULL) return;

	LNode traverse = head;

	while(traverse != NULL){
    int flag = 1;
		if(traverse->changed < 0){
			struct stat T;
			if(stat(traverse->fullpath,&T) == -1)
				return;
			
			if(S_ISDIR(T.st_mode)){
				delete_directory(records, traverse->fullpath);
        flag = 0;
			}
      
      if(vflag && flag) printf("File %s deleted from directory\n",traverse->fullpath);
      remove(traverse->fullpath);
		}

		traverse = traverse->next;
	}
}

//print list contents
void list_print(LNode head){
	if(head == NULL) return;

	LNode traverse = head;

	while(traverse != NULL){
		printf("%s\n",head->fullpath);
		printf("%d\n",head->changed);
		traverse = traverse->next;
	}
}

//Free allocated memory for list
void list_free(LNode head){
	if(head == NULL) return;

	list_free(head->next);
	free_filent(head->info);
	free(head->fullpath);
	free(head);
}