#ifndef QUIC_H
#define QUIC_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include "structs.h"

#define BUFFSIZE 1024

int files_seen;
int file_copied;
off_t total_bytes;
int vflag;

void argument_check(int argc, char* argv[]);
void cp_function(char *origin, char* dest, int lflag);
void cp_existing_function(char *origin, char* dest, int lflag, int dflag);
void search_directory(char* origin, char* dest, int lflag);
void create_and_copy(filent dir_A, char* origin, char* dest, int lflag);
void delete_directory(HTable records, char* path);
void initialize_global_variables(void);
void inode_info(char* name);

#endif