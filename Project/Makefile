CC=gcc
CFLAGS = -Wall -g -pthread
DEPS = hash.h list.h header.h acutest.h hashtable.h job_scheduler.h
OBJ = main.o job_scheduler.o implementation.o hash.o cliquelist.o recordlist.o hashlist.o help.o hashtable.o machinelearning.o
OBJ1 = unit_test.o job_scheduler.o implementation.o hash.o cliquelist.o recordlist.o hashlist.o help.o hashtable.o machinelearning.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: project unit_test

project: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

unit_test: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS) -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) project $(OBJ1) unit_test

run: project unit_test
	valgrind --leak-check=full ./unit_test
	valgrind --leak-check=full --track-origins=yes ./project
