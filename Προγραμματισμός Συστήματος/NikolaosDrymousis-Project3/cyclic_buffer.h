#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

typedef struct cyclic_buffer{
  char** cyclic;
  int buffer_size;
  int end;
  int count;
  int start;
}cyclic_buffer, *CBUFFER;

CBUFFER initialize_cyclic_buffer(int size);
void add_to_buffer(char* word);
char* remove_from_buffer(void);
void* thread_function(void *ptr);
void free_cyclic_buffer(void);


#endif