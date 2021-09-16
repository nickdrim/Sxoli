#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <poll.h>
#include <unistd.h>

#include "hash.h"
#include "citizen_hash.h"
#include "visited.h"
#include "pid_hash.h"

#define default_bloom_size 1000
#define default_bloom_string_size "1000"

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
#define DEFAULT_BUFFER 15
#define DEFAULT_BUFFER_STRING "15"
#define DEFAULT_CHILD 4;

int argument_check(int argc, char* argv[]);
int no_of_arguments(int current, int expected, int extra);
int hash(char* key, int table_size);
int is_string(char *word);
int is_date(char* word);
int is_integer(char* word);
int is_valid_age(char* word);
int yes_or_no(char* word);
int count_rows(char* path, int size);
char* create_date(void);
int compareDate(char *rootString , char *newString);
void free_array_contents(char** arr, int size);
void parser(HTable records, CHTable citizen_records, char* word, int size);
void signal_handler(int sig);
void monitor_signal_handler(int sig);
void read_fifo(void);
void logger(int id, char** countries, int size, int requests,int accepted, int denied);
char* vaccineStatus(HTable records, char* citizenID, char* virusName, char* arrival_date, int write_fd);
void citizenStatus(HTable records, CHTable citizens ,char* citizenID, int fpipe);
void initialize_read_file(void);
int six_months_prior(char* vaccin_date, char* travel_date);
void travelStats(PIDHASH records, char* desease, char* start_date, char* end_date, char* country, int pid);
void command_line(void);
int bloom_size_get(void);
void set_bloom_size(int size);

#endif