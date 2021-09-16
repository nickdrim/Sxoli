#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/times.h>
#include <math.h>

#include <fcntl.h>           /* Definition of AT_* constants */

#include "list.h"
#include "help.h"

int processes(int lowerbound, int upperbound, int noProcesses);

#endif