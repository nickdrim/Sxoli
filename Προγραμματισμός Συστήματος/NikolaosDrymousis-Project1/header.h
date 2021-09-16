#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <ctype.h>
#include <time.h>

#include "hash.h"
#include "citizen_hash.h"
#include "population.h"

#define default_bloom_size 100000

//Name for index of a record table to be easier to read
#define citizen 0
#define fname 1
#define lname 2
#define location 3
#define years 4
#define virus_id 5
#define yes_no 6
#define vac_date 7

//Default bloom filter K 
#define K 16

int argument_check(int argc, char* argv[]);
int no_of_arguments(int current, int expected, int extra);
int is_string(char *word);
void command_line(HTable records, CHTable citizen_records, PHash country_records);
int is_date(char* word);
int is_integer(char* word);
int is_valid_age(char* word);
int yes_or_no(char* word);
int count_rows(char* path, int size);
char* create_date(void);
int compareDate(char *rootString , char *newString);
void free_array_contents(char** arr, int size);
void parser(HTable records, CHTable citizen_records, PHash country_records, char* word, int size);

LNode vaccineStatusBloom(HTable records, char* citizenID, char* virusName,int flag);
void vaccineStatus(HTable records, char* citizenID, char* virusName);
void vaccinateNow(HTable records, CHTable citizen_records, char** item);
void insertCitizenRecord(HTable records, CHTable citizen_records, char** commands, int commands_size, int flag);
void listNonVaccinatedPersons(HTable records, char* virusName);
void populationStatus(HTable records, PHash country_records, char** commands, int commands_size);
void popStatusByAge(HTable records, PHash country_records, char** commands, int commands_size);

#endif