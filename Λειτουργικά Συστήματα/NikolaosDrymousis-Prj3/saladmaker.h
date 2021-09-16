#ifndef SALADMAKER_H
#define SALADMAKER_H

#include "shared_memory.h"

#define AVAILABLE 1
#define UNAVAILABLE 0

typedef struct ingredients{
  int tomato;
  int pepper;
  int onion;
  char* bag_ing;
}ingredients, *ing_ptr;

void usr1_handler(int sig);
void error_handler(int sig);
int argument_check(int argc, char* argv[]);

#endif