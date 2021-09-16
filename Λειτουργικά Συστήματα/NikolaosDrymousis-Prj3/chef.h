#ifndef CHEF_H
#define CHEF_H

#include "shared_memory.h"

int argument_check(int argc, char* argv[]);
void usr1_handler(int sig);
void interrupt_handler(int sig);

#endif