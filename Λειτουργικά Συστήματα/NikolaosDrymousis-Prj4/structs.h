#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct file_info{
  ino_t ino;
  off_t off;
  unsigned short reclen;
  char* name;
}file_info,*filent;

typedef struct tree{
  char* folder;
  filent *files;
  struct tree** subfolders;
  int file_counter;
  int subfolder_counter;

}tree,*TNode;

typedef struct list{
  filent info;
  char* fullpath;
  int changed;
  struct list* next;
}list,*LNode;

typedef struct hashtable{
  LNode start;
}hashtable,*HTable;

int hash(char* key);
HTable hash_initialize(void);
void hash_insert(HTable records, struct dirent* data, char* path);
void hash_search_update(HTable records, char* dest, char* name, int change);
void hash_directory_search(HTable records, char* path);
void hash_free(HTable records);
void hash_directory_insert(TNode* root, HTable records, char* dest);
void hash_delete_unchanged_files(HTable records);
void hash_search_deleted(HTable records, char* path);
void hash_print(HTable records);

LNode new_list_node(filent item, char* path);
void list_insert(LNode* head, LNode data);
void list_search_update(LNode head, char* dest, char* name, int change);
void list_directory_search(LNode head, char* path);
void list_free(LNode head);
void list_delete_unchanged_files(HTable records, LNode head);
void list_search_deleted(LNode head, char* path);
void list_print(LNode head);

TNode new_tree_node(char* data);
void tree_free(TNode root);
void compare_directories(TNode origin_root, TNode dest_root, HTable dest_records, char* origin, char* dest, int lflag);
void inorder_print(TNode root);

filent create_filent(struct dirent* data);
void free_filent(filent data);

#endif