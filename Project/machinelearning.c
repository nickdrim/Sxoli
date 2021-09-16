#include "header.h"
#include "time.h"

ht_w *ht;
int json_file_words=0;
int word_in_json_file=0;

int check_char(char sample, int check){
	if(!check){
		if((sample == '\"' || sample == '-' || sample == '<' || sample == '>' || sample == ':' || sample == '\'' || sample == ',' || sample == '.' ||
				sample == '[' || sample == ']' || sample == '{' || sample == '}' || sample == '(' || sample == ')' || sample == ' ' || 
				sample == '\\' || sample == '/' || sample == '?' || sample == '+' || sample == '=' || sample =='&' || sample == '!' || (sample>='0' && sample<='9') ||
				sample == '\n' || sample == '\t')){
					return 1;
		}
	}else{
		if(sample == '\"' || sample == '-' || sample == '<' || sample == '>' || sample == ':' || sample == '\'' || sample == ',' || sample == '.' ||
			sample == '[' || sample == ']' || sample == '{' || sample == '}' || sample == '(' || sample == ')' || sample == ' ' || 
			sample == '\\' || sample == '/' || sample == '?' || sample == '+' || sample == '=' || sample == '&' || sample == '!' || sample == '\n' || sample == '\t'){
				return 1;
		}
	}

	return 0;
}

void parser(char* path,char* fullpath, int size,int x){
	entry_w** array = NULL;
	int array_size = 0;
	char *temp = NULL;
	int word_size = 0;
	int total = 0;
	for(int i = 0 ; i<size ; i++){
		if((check_char(path[i],0)) && temp == NULL){
				
			continue;
		}else if((check_char(path[i],1)) && temp != NULL){
			if(strlen(temp) == 1){
				free(temp);
				word_size = 0;
				temp = NULL;
				continue;
			}
			total++;
			if(array == NULL){
				array = malloc(sizeof(entry_w *));
				array[0] = ht_insert(ht,temp,fullpath,x);
				array_size++;
			}else{
				entry_w* exists = ht_insert(ht,temp,fullpath,x);
				if(exists != NULL){
					array_size++;
					array = realloc(array, sizeof(entry_w *) * array_size);
					array[array_size - 1] = exists;
				}
			}

			free(temp);
			temp = NULL;
			word_size = 0;
		}else{
			char lower = path[i];
			if(lower >= 'A' && lower <='Z')
				lower = tolower(lower);
      	
			if(temp == NULL){
				word_size++;
				temp = malloc(sizeof(char)*word_size + 1);
				temp[0] = lower;
				temp[1] = '\0';
			}else{
				word_size++;
				char* swap = NULL;
				swap = realloc(temp,strlen(temp) + 2);
				temp = swap;
				temp[word_size-1] = lower;
				temp[word_size] = '\0';
			}
		}
	}

	if(array != NULL){
		for(int i = 0 ; i < array_size ; i++){
			update_entry(array[i], 0, total);
		}
		free(array);
	}
}


ht_w* BagOfWords(){
	clock_t begin = clock();
	ht=ht_create(0);
	DIR *folder;
	DIR *subfolder;
//	FILE *fp;
	struct dirent *in_folder;
	struct dirent *in_subfolder;
	char c[50]={'\0'};
	char hash[50];
//	char hashname[50];
	char subpath[50];
	char path[50];

	folder=opendir("2013_camera_specs");
	if(folder==NULL){
		printf("****Failed to open directory****\n");
		exit(0);
	}
	int counter = 0;

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
			memset(path,'\0',50);
			if(!strcmp(in_subfolder->d_name,"."))
				continue;
			if(!strcmp(in_subfolder->d_name,".."))
				continue;
			
			counter++;
			strcpy(path,subpath);
			strcat(path,in_subfolder->d_name);

			int fd;
			if((fd = open(path, O_RDONLY)) < 0){
				perror("line 147 : BOW open\n");
			}
			struct stat F;
			stat(path,&F);

			char* mmap_file;
			if((mmap_file = mmap(NULL, F.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
				perror("line 154 : Bag of words mmap ");
			}

			parser(mmap_file, path ,F.st_size ,counter);
			close(fd);
			munmap(mmap_file, F.st_size);
			//readfile(mmap_file);
//			CALL HASH CREATE FUNCTION !!!!!!!*****
//			****PATH VARIABLE CONTAINS THE WHOLE PATH OF JSON FILE FOR FUTURE USE

		}
		closedir(subfolder);
	}
	closedir(folder);
	ht_calculate_tf_idf(ht);
	printf("\n\n\n******COMPLETED BOW******\n\n\n");
	printf("\n\n\n******COMPLETED TF-IDF******\n\n\n");

	clock_t end = clock();
	printf("Completed Bag of words in %f\n", (double)(end - begin) / CLOCKS_PER_SEC);


	return create_smaller_ht(ht);
}

int* vectorise(char* file1, char* file2, ht_w* word_hash){
	if(word_hash == NULL){
		int* temp = malloc(sizeof(int) * 2 * vector_size());

		for(int i = 0 ; i < 2*vector_size() ; i++){
			temp[i] = 0;
		}

		return temp;
	}

	int* temp = malloc(sizeof(int) * 2 * vector_size());

	for(int i = 0 ; i < 2*vector_size() ; i++){
		temp[i] = 0;
	}

	char* file_copy1 = malloc(strlen("2013_camera_specs") + strlen(file1) + 2);
	strcpy(file_copy1,"2013_camera_specs");
	strcat(file_copy1,"/");
	strcat(file_copy1,file1);

	char* file_copy2 = malloc(strlen("2013_camera_specs") + strlen(file2) + 2);
	strcpy(file_copy2,"2013_camera_specs");
	strcat(file_copy2,"/");
	strcat(file_copy2,file2);


	char* files[2];
	files[0] = file_copy1;
	files[1] = file_copy2;

	for(int i = 0 ; i <= 1 ; i++){
		int fd;
		if((fd = open(files[i], O_RDONLY)) < 0){
			perror("ok\n");
		}
		
		struct stat F;
		stat(files[i],&F);

		char* mmap_file;
		if((mmap_file = mmap(NULL, F.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
			perror("wrf ");
		}

		if(i){
			find_and_vectorise(word_hash,mmap_file, temp, 0, F.st_size);
		}else{
			find_and_vectorise(word_hash,mmap_file, temp, 1000, F.st_size);
		}
		close(fd);
		munmap(mmap_file, F.st_size);
	}

	free(file_copy1);
	free(file_copy2);

	return temp;
}

void find_and_vectorise(ht_w* ht, char* file, int* vector,int mul,int size){
	if(file == NULL){
		printf("file empty\n");
		return;
	}

	char *temp = NULL;
	int word_size = 0;
	for(int i = 0 ; i < size ; i++){
		if((check_char(file[i],0)) && temp == NULL){
				
			continue;
		}else if((check_char(file[i],1)) && temp != NULL){
			if(strlen(temp) == 1){
				free(temp);
				word_size = 0;
				temp = NULL;
				continue;
			}
			
			if(ht_search(ht, temp) > -1){
				vector[ht_search(ht, temp) + mul]++;
			}

			free(temp);
			temp = NULL;
			word_size = 0;
		}else{
			char lower = file[i];
			if(lower >= 'A' && lower <='Z')
				lower = tolower(lower);
      	
			if(temp == NULL){
				word_size++;
				temp = malloc(sizeof(char)*word_size + 1);
				temp[0] = lower;
				temp[1] = '\0';
			}else{
				word_size++;
				char* swap = NULL;
				swap = realloc(temp,strlen(temp) + 2);
				temp = swap;
				temp[word_size-1] = lower;
				temp[word_size] = '\0';
			}
		}
	}
}