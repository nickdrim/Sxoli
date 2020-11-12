#ifndef HELP_H
#define HELP_H

#include <stdarg.h>
#include "hashTable.h"
#include "linked_list.h"

extern int size_of_disease;
extern int size_of_countries;

void menu(FILE *fp , int diseaseSize , int countrySize , int BucketSize);
void command_guide();
void initialize(FILE *fp , HTable disease , HTable countries , LNode *head , LNode *last);

int validDate(char *word);
int validInt(char *word);
int isString(char *word);
void commands();
int compareDate(char *rootString , char *newString);

void globalDiseaseStats(HTable disease , const char *str,...);
void recordPatientExit(LNode head , int id , char* exit);
void numCurrentPatients(HTable disease , char* target);
int diseaseFrequency(HTable disease , char *virusName , char* start , char* end , char* place);
void topk_Diseases(HTable disease , int k , char *place , char *start , char *end );
void topk_Countries(HTable countries , int k , char *virusName , char *start , char *end );
void programmExit(LNode head , HTable disease , HTable countries);

#endif