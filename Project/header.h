#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#include "hashtable.h"
#include "hash.h"
#include "help.h"
#include "job_scheduler.h"

#define EMPTY -100
#define BATCH_SIZE 512

//Functions_Headers
void reading_dataset_x(void);
full_set_ptr reading_dataset_w(CNode head, HTable records);
int count_rows(FILE *);
int count_all_jsonfiles(void);
void free_structures(CNode head, HTable records, ht_w* ht, full_set_ptr w_set);
void isEqual(CNode head, HTable records, char* ID1, char* ID2, int flag);
void cliqueListPrint(CNode head);
void cliqueListInsert(CNode *head, HTable records, char* ID, char* location);
ht_w* BagOfWords(void);


void parser(char* path, char*fullpath, int size,int x);
int* vectorise(char* file1,char* file2, ht_w *word_hash);
void find_and_vectorise(ht_w* ht, char* file, int* vector, int mul, int size);
int check_char(char sample,int check);