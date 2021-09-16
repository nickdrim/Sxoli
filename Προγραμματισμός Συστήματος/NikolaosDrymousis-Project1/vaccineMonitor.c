#include "header.h"

int bsize = 0;
int fd = 0;
char* file = NULL;

int main(int argc, char *argv[]){
  srand(time(NULL));
  if(argument_check(argc,argv)){
    exit(1);
  }

  struct stat F;
  stat(file,&F);

  char* mmap_file;
  if((mmap_file = mmap(NULL, F.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
    perror("line 18 : mmap ");
    exit(1);
  }

  int rows = count_rows(mmap_file, F.st_size);

  //Initialize structures
  HTable records = hash_initialize(rows, bsize);
  CHTable citizen_records = citizen_hash_initialize(rows);
  PHash country_records = pop_hash_initialize();

  //Parse file
  parser(records, citizen_records,country_records ,mmap_file, F.st_size);

  munmap(mmap_file, F.st_size);
  //hash_print(records);

  command_line(records, citizen_records, country_records);

  hash_free(records);
  citizen_hash_free(citizen_records);
  pop_hash_free(country_records);

  return 0;
}

//return rows of given file
int count_rows(char* path, int size){
  int rows = 0;
	for(int i = 0 ; i < size ; i++){
		if(path[i] == '\n')
			rows = rows + 1;
	}
	return rows;
}

//Checks if arguments are correct
int argument_check(int argc, char* argv[]){
  if(argc < 5){
    printf("Not enough arguments\n");
    return 1;
  }else if(argc > 5){
    printf("Too many arguments\n");
    return 1;
  }else{
    int cflag = 0;
    int bflag = 0;
    for(int i = 1 ; i < argc ; i += 2){
      if(!strcmp(argv[i],"-c")){
        cflag++;
        if(cflag == 2){
          printf("-c flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-b")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        if((fd = open(argv[i+1], O_RDONLY)) == -1){
          perror("file :");
          return 1;
        }
        file = argv[i+1];
      }else if(!strcmp(argv[i],"-b")){
        bflag++;
        if(bflag == 2){
          printf("-b flag is repeated\n");
          return 1;
        }
        if(!strcmp(argv[i+1],"-c")){
          printf("Arguments are in the wrong order\n");
          return 1;
        }
        for(int j = 0 ; j < strlen(argv[i+1]) ; j++){
          if(!isdigit(argv[i+1][j])){
            printf("Argument after -b flag must be a number\n");
            return 1;
          }
        }
        bsize = atoi(argv[i+1]);
      }
    }

    return 0;
  }
}

//parses file and inserts records to structures
void parser(HTable records, CHTable citizen_records, PHash country_records, char* word, int size){
  
  int array_size = 8;
  char** array = malloc(sizeof(char*) * array_size);

  for(int i = 0 ; i < array_size ; i++){
    array[i] = NULL;
  }

  char* temp = NULL;
  int word_size = 0;
  int counter = 0;
  int flag = 0;         //error flag 

  for(int i = 0 ; i < size ; i++){
    if(flag){
      if(word[i] == '\n'){
        //if all row is read
        flag = 0;
        counter = 0;
      }else{
        //ignore until new row
        continue;
      }
    }
    else if(word[i] == ' ' && temp == NULL){
      //if white space and no word
      continue;
    }else if((word[i] == ' ' || word[i] == '\n') && temp != NULL){
      //if word is complete
      word_size = 0;

      //depending on the number of words already read in a row pick a case
      switch (counter){
        case 0:
        //check id
          if(is_integer(temp)){
            array[counter] = temp;
            temp = NULL;
          }else{
            printf("error with record id\n");
            flag = 1;
          }
          break;
        case 1:
        //check if first name is string
          if(is_string(temp)){
            array[counter] = temp;
            temp = NULL;
          }else{
            printf("error with record first name\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 2:
        //check if last name is string
          if(is_string(temp)){
            array[counter] = temp;
            temp = NULL;
          }else{
            printf("error with record last name\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 3:
        //check if country is string
          if(is_string(temp)){
            array[counter] = temp;
            temp = NULL;
          }else{
            printf("error with record country\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 4:
        //check if age is integer and in valid range
          if(is_integer(temp) && is_valid_age(temp)){
            array[counter] = temp;
            temp = NULL;
          }else{
            printf("error with record age %s\n", temp);
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 5:
        //can be alpharithmetic so no need to check anything
          array[counter] = temp;
          temp = NULL;
          break;
        case 6:
        //check if answer is yes or no
          if(yes_or_no(temp)){
            array[counter] = temp;
            temp = NULL;
            //if no record is ready. Insert it to structs
            if(!strcmp(array[counter],"NO")){
              hash_insert(records, array);
              citizen_hash_insert(citizen_records, array);
              pop_hash_insert(country_records, array[location], atoi(array[years]));
            }
          }else{
            printf("error with record answer\n");
            flag = 1;
            free_array_contents(array, array_size);
          }
          break;
        case 7:
        //Check if previous string was yes
          if(!strcmp(array[counter - 1], "YES")){
            //check if date is valid
            if(is_date(temp)){
              array[counter] = temp;
              temp = NULL;
            }else{
              printf("error with record date\n");
              flag = 1;
              free_array_contents(array, array_size);
            }
          }else{
            printf("error with record answer/date %s\n",temp);
            flag = 1;
            free_array_contents(array, array_size);
          }
          //if no errors and all words are read, insert record to structs
          if(!flag){
            //if id already exists
            if(citizen_hash_search(citizen_records, array) == 1){
              hash_insert(records, array);
            }else if(citizen_hash_search(citizen_records, array) == 2){
              //if id is new
              citizen_hash_insert(citizen_records, array);
              hash_insert(records, array);
              pop_hash_insert(country_records, array[location], atoi(array[years]));
            }else{
              //if id and info do not match
              printf("error with record. Records with id %s exists with other name/lastname/age/country\n",array[citizen]);
              free_array_contents(array, array_size);
            }
          }
          break;
        default:
          break;

      }
      counter++;
      //if record is complete and inserted,reset values for new records.
      if(counter == 8 || (counter == 7 && !strcmp(array[yes_no],"NO"))){

        //uncomment to print file containts to see parsers effectiveness
        /*for(int j = 0 ; j < array_size-1 ; j++){
          printf("%s ",array[j]);
        }
        printf("\n");*/
        free_array_contents(array, array_size);
        counter = 0;
      }
    }else if(word[i] == '\n'){
      //if new line before neccesary info delete containts
      if(counter < 7){
        free_array_contents(array, array_size);
        printf("Error with record.Incomplete\n");
      }
    }else{
      //if new word create it.
      if(temp == NULL){
        word_size++;
        temp = malloc(sizeof(char)*word_size + 1);
				temp[0] = word[i];
				temp[1] = '\0';
      }else{
        //else expand it until whitespace
        word_size++;
				temp = realloc(temp,strlen(temp) + 2);
				temp[word_size-1] = word[i];
				temp[word_size] = '\0';
      }
    }
  }

  free(array);
}

//deallocate memory from an array
void free_array_contents(char** arr, int size){
  for(int j = 0 ; j < size ; j++){
    if(arr[j] != NULL) free(arr[j]);
    arr[j] = NULL;
  }
}