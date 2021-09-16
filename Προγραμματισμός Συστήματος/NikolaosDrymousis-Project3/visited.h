#ifndef VISITED_H
#define VISITED_H

typedef struct directory{
  char* dirname;
  char* country;
  char** filenames;
  int no_files;
  struct directory* next;
}directory,*dir_ptr;

typedef struct dir_hash{
  dir_ptr start;
}dir_hash, *DHash;

DHash dir_hash_initialize(void);
dir_ptr new_dir_node(char* name);
void dir_hash_insert(DHash directories, char* name);
void dir_hash_list_insert(dir_ptr* head, char* name);
int dir_hash_search_insert(DHash directories, char* dname, char* filname);
int dir_hash_list_search_insert(dir_ptr head, char* dname, char* filname);
void dir_hash_free(DHash directories);
void dir_hash_list_free(dir_ptr head);
char** dir_hash_return_countries(DHash directories, int *size);
int dir_hash_update_directories(DHash directories, HTable records, CHTable citizen_records);

#endif