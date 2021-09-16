#include "header.h"

#define W "Dataset_W.csv"
#define Y "sigmod_medium_labelled_dataset.csv"


full_set_ptr reading_dataset_w(CNode head, HTable records){

	time_t t;
	srand((unsigned) time(&t));
	FILE *fp = NULL;
	char buff[255];
	char *string,*label;
	char string1[50]={'\0'};
	char string2[50]={'\0'};

	//Opening file
	fp=fopen(W,"r");
	if(fp==NULL){
		printf("****Can't open file or file doesn't exist.****");
		exit(0);
	}

	//Counting rows of file
	int rows=count_rows(fp);
	rewind(fp);
	fscanf(fp,"%s",buff);
	
	full_set_ptr w_set = create_full_set(rows-1);
	int train_counter = 0;
	int test_counter = 0;
	int valid_counter = 0;
	int random = 0;

	//Reading attributes

	for(int i=1;i<rows;i++){
		random = rand() % 3;
		fscanf(fp,"%s",buff);
		char *temp = malloc(strlen(buff)+1);
		char *ftemp = temp;
		strcpy(temp,buff);

		string=strsep(&temp,",");
		strcpy(string1,string);
		strcat(string1,".json");

		string=strsep(&temp,",");
		strcpy(string2,string);
		strcat(string2,".json");

		label=strsep(&temp,",");
		int val=atoi(label);
		//printf("%s\t%s\t%d\n",string1,string2,val);
		free(ftemp);

		if(random == 0){
			if(train_counter <= w_set->training_size){
				w_set->training_set[train_counter] = create_set(string1,string2,val);
				train_counter++;
			}else{
				random = rand() % 2;
				if(random == 0){
					if(test_counter < w_set->test_size){
						w_set->test_set[test_counter] = create_set(string1,string2,val);
						test_counter++;
					}
				}else{
					if(valid_counter < w_set->validation_size){
						w_set->validation_set[valid_counter] = create_set(string1,string2,val);
						valid_counter++;
					}
				}
			}
		}else if(random == 1){
			if(test_counter < w_set->test_size){
				w_set->test_set[test_counter] = create_set(string1,string2,val);
				test_counter++;
			}else{
				random = rand() % 2;
				if(random == 0){
					if(train_counter < w_set->training_size){
						w_set->training_set[train_counter] = create_set(string1,string2,val);
						train_counter++;
					}
				}else{
					if(valid_counter < w_set->validation_size){
						w_set->validation_set[valid_counter] = create_set(string1,string2,val);
						valid_counter++;
					}else if(train_counter < w_set->training_size){
						w_set->training_set[train_counter] = create_set(string1,string2,val);
						train_counter++;
					}
				}
			}
		}else{
			if(valid_counter < w_set->validation_size){
				w_set->validation_set[valid_counter] = create_set(string1,string2,val);
				valid_counter++;
			}else{
				random = rand() % 2;
				if(random == 0){
					if(train_counter < w_set->training_size){
						w_set->training_set[train_counter] = create_set(string1,string2,val);
						train_counter++;
					}
				}else{
					if(test_counter < w_set->test_size){
						w_set->test_set[test_counter] = create_set(string1,string2,val);
						test_counter++;
					}else if(train_counter < w_set->training_size){
						w_set->training_set[train_counter] = create_set(string1,string2,val);
						train_counter++;
					}
				}
			}
		}

		
		if(val){
			isEqual(head, records, string1, string2,val);
		}else if(!val){
			isEqual(head, records, string1, string2,val);
		}
		
	}

	fclose(fp);
	printf("\n\n\n******COMPLETED READING OF DATASET_W******\n\n\n");
	return w_set;
}

int count_rows(FILE *fp){

	int rows=0;
	char c;
	for(c=getc(fp); c!=EOF; c=getc(fp)){
		if(c=='\n')
			rows=rows+1;
	}
	return rows;
}


void reading_dataset_x(void){

	DIR *folder;
	DIR *subfolder;
	struct dirent *in_folder;
	struct dirent *in_subfolder;
	char c[50]={'\0'};
	char hash[50];
	char hashname[50];
	char subpath[50];
	char path[50];

	folder=opendir("2013_camera_specs");
	if(folder==NULL){
		printf("****Failed to open directory****\n");
		exit(0);
	}

	HTable records = hashInitialize(count_all_jsonfiles());
	CNode head = NULL;

	while((in_folder=readdir(folder)) != NULL){
		memset(hash,'\0',50);
		memset(subpath,'\0',50);
		if(!strcmp(in_folder->d_name, "."))
			continue;
		if(!strcmp(in_folder->d_name, ".."))
			continue;
		strcpy(c,"2013_camera_specs/");
		strcat(c,in_folder->d_name);

		strcpy(subpath,c);
		strcat(subpath,"/");

		strcpy(hash,in_folder->d_name);
		strcat(hash,"/");
		strcat(hash,"/");

		subfolder=opendir(c);
		if(subfolder==NULL){
			printf("****Failed to open subfolder****\n");
			exit(0);
		}
		while((in_subfolder=readdir(subfolder)) !=NULL){
			memset(hashname,'\0',50);
			memset(path,'\0',50);
			if(!strcmp(in_subfolder->d_name,"."))
				continue;
			if(!strcmp(in_subfolder->d_name,".."))
				continue;
			strcpy(hashname,hash);
			strcpy(path,subpath);

			strcat(path,in_subfolder->d_name);
			strcat(hashname,in_subfolder->d_name);
			//printf("%s\t%s\n",hashname,path);

			cliqueListInsert(&head, records ,hashname, path);

		}
		
		closedir(subfolder);
	}
	closedir(folder);
	printf("\n\n\n******COMPLETED READING OF DATASET_X******\n\n\n");
	full_set_ptr w_set = reading_dataset_w(head, records);
	create_negative_relationships(records, w_set);
	
	cliqueListPrint(head);
	count_and_print_negative(head);
	//print_clique_array(visited);
	ht_w* words_hash = BagOfWords();
	//ht_new_print(words_hash,10000);
	
	//wprint(w_set);
	first_training(w_set, words_hash, records);

	train_test_validate(w_set, words_hash);

	free_structures(head,records,words_hash,w_set);
}

void free_structures(CNode head, HTable records, ht_w* ht,full_set_ptr w_set){
	printf("\nDESTROYING STRUCTURES\n");
	cliqueFreeList(head);
	hashFree(records);
	ht_free(ht, 10000);
	free_full_set(w_set);
	printf("\nSTRUCTURES DESTROYED\n");
}

void isEqual(CNode head, HTable records, char* ID1, char* ID2, int flag){
	CNode temp1 = hashSearch(records, ID1);
	CNode temp2 = hashSearch(records, ID2);
	if(temp1 == NULL || temp2 == NULL){
		return;
	}
	if(temp1->clique->info == temp2->clique->info){
		return;
	}
	
	if(flag){
		if(strcmp(temp1->clique->info->name,temp2->clique->info->name) < 0){
			recordListEqualInsert(head, temp1->clique,&(temp2->clique),temp2);
		}else if(strcmp(temp1->clique->info->name,temp2->clique->info->name) > 0){
			recordListEqualInsert(head, temp2->clique,&(temp1->clique),temp1);
		}else{
			return;
		}
	}/*else{
		negative_relationship(temp1->clique,temp2->clique);
	}*/

}

void cliqueListPrint(CNode head){
  if(listEmpty(head)) return;

  pbucket *visited = NULL;
  int currentSize = 0;

  CNode traverse = head;
  int i=1;

  while(traverse!=NULL){
		int count = 0;
		int check = 1;
		int flag = 1;
		RNode temp = traverse->clique->info;

		if(!listEmpty(traverse->clique) && (traverse->clique->info->next!=NULL)){
			if(visited == NULL){
				currentSize++;
				visited = malloc(sizeof(pbucket)*currentSize);
				visited[0] = traverse->clique;
			}else{
				for(int j=0 ; j < currentSize ; j++){
					if(visited[j] == traverse->clique){
						flag = 0;
						break;
					}
				}
				if(flag){
					currentSize++;
					visited = realloc(visited,sizeof(pbucket)*currentSize);
					visited[currentSize-1] = traverse->clique;
				}
			}
			if(flag&&temp->next!=NULL){
				printf("Clique %d : ",i);
				i++;
			}else{
				check = 0;
				flag = 0;
			}
			
		}else check = 0;

		if(flag && check){
			RNode current = traverse->clique->info;
			while(current != NULL){
				temp = current->next;
				while(temp!=NULL){
					if(current == traverse->clique->info) count++;
					if(current != temp){
						if(temp->next != NULL) printf("(%s, %s )-> ", current->name, temp->name);
						else printf("(%s, %s)\t", current->name, temp->name);
					}
					temp = temp->next;
				}
				current = current->next;
			}
		
		}
		if(check){
			printf("\nUnique items in clique = %d\n\n\n",count);
			traverse->clique->no_items = count;
		}
    
    traverse = traverse->right;
  }

	free(visited);
}

int count_all_jsonfiles(void){
	DIR *folder;
	DIR *subfolder;
	struct dirent *in_folder;
	struct dirent *in_subfolder;
	int counter=0;
	char c[50]={'\0'};

	folder=opendir("2013_camera_specs");
	if(folder==NULL){
					printf("****Failed to open directory****\n");
					return 0;
	}
	while((in_folder=readdir(folder)) != NULL){
					if(!strcmp(in_folder->d_name, "."))
									continue;
					if(!strcmp(in_folder->d_name, ".."))
									continue;
					strcpy(c,"2013_camera_specs/");
					strcat(c,in_folder->d_name);

					subfolder=opendir(c);
					if(subfolder==NULL){
									printf("****Failed to open subfolder****\n");
									return 0;
					}
					while((in_subfolder=readdir(subfolder)) !=NULL){
									if(!strcmp(in_subfolder->d_name,"."))
													continue;
									if(!strcmp(in_subfolder->d_name,".."))
													continue;
									counter++;
					}
					closedir(subfolder);
	}
	closedir(folder);
	return counter;
}

