#include "header.h"



void reading_dataset_w(CNode head, HTable records){

	FILE *fp;
	char buff[255];
	char *string,*label;
	char string1[50]={'\0'};
	char string2[50]={'\0'};
	int rows;

	//Opening file
	fp=fopen("Dataset_W.csv","r");
	if(fp==NULL){
		printf("****Can't open file or file doesn't exist.****");
		exit(0);
	}

	//Counting rows of file
	rows=count_rows(fp);
	rewind(fp);
	fscanf(fp,"%s",buff);

	//Reading attributes
	for(int i=1;i<=rows;i++){
		fscanf(fp,"%s",buff);
		char *temp = malloc(255);
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
		
		if(val==1){
			isEqual(head, records, string1, string2);
		}
		/*else
			next_project
		*/
	}
	fclose(fp);
	printf("\n\n\n******COMPLETED READING OF DATASET_W******\n\n\n");
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
	FILE *fp;
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

//		printf("%s\n",c);
//		printf("%s\n",hash);

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

//			CALL HASH CREATE FUNCTION !!!!!!!*****
//			create_Hashtable_LinkedList(hashname,path);
//			****HASHNAME VARIABLE CONTAINS THE HASH_KEY
//			****PATH VARIABLE CONTAINS THE WHOLE PATH OF JSON FILE FOR FUTURE USE

		}
		
		closedir(subfolder);
	}
	closedir(folder);
	printf("\n\n\n******COMPLETED READING OF DATASET_X******\n\n\n");
	reading_dataset_w(head, records);
	cliqueListPrint(head);

	free_structures(head,records);
}

void free_structures(CNode head, HTable records){
	cliqueFreeList(head);
	hashFree(records);
}

void isEqual(CNode head, HTable records, char* ID1, char* ID2){
	CNode temp1 = hashSearch(records, ID1);
	CNode temp2 = hashSearch(records, ID2);

	if(temp1 == NULL || temp2 == NULL){
		return;
	}
	if(temp1->info == temp2->info){
		return;
	}
	if(strcmp(temp1->info->name,temp2->info->name) < 0){
		recordListEqualInsert(head, temp1->info,&(temp2->info),temp2);
	}else if(strcmp(temp1->info->name,temp2->info->name) > 0){
		recordListEqualInsert(head, temp2->info,&(temp1->info),temp1);
	}else{
		return;
	}
}

void cliqueListPrint(CNode head){
  if(listEmpty(head)) return;

  RNode *visited = NULL;
  int currentSize = 0;

  CNode traverse = head;
  int i=1;

  while(traverse!=NULL){
    RNode temp = traverse->info;
    int check = 1;
    int flag = 1;

    if(!listEmpty(temp)){
      if(visited == NULL){
        currentSize++;
        visited = malloc(sizeof(RNode)*currentSize);
        visited[0] = temp;
      }else{
        for(int j=0 ; j < currentSize ; j++){
          if(visited[j] == temp){
            flag = 0;
          }
        }
        if(flag){
          currentSize++;
          visited = realloc(visited,sizeof(RNode)*currentSize);
          visited[currentSize-1] = temp;
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

    if(flag){
			int count = 0;
      while(temp!=NULL){
				count++;
        if(temp->next != NULL) printf("%s -> ", temp->name);
				else printf("%s\t", temp->name);
        temp = temp->next;
				if(temp!=NULL){
					currentSize++;
					visited = realloc(visited,sizeof(RNode)*currentSize);
					visited[currentSize-1] = temp;
				}
      }
      if(check) printf("Total identical files = %d\n\n\n",count);
    }
    
    traverse = traverse->right;
  }

	for(int i=0 ; i < currentSize ; i++){
		visited[i] = NULL;
	}
  free(visited);
}



int count_all_jsonfiles(){
        DIR *folder;
        DIR *subfolder;
        FILE *fp;
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

