#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "hash.h"

//Functions_Headers
void reading_dataset_x(void);
void reading_dataset_w(CNode head, HTable records);
int count_rows(FILE *);
int count_all_jsonfiles();
void free_structures(CNode head, HTable records);
void isEqual(CNode head, HTable records, char* ID1, char* ID2);
void cliqueListPrint(CNode head);
void cliqueListInsert(CNode *head, HTable records, char* ID, char* location);
