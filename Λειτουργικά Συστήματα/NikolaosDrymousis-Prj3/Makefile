CC=gcc
CFLAGS = -Wall -g -lm -pthread
DEPS = shared_memory.h saladmaker.h chef.h help.h
OBJ = chef.o shared_memory.o help.o
OBJ1 = saladmaker.o shared_memory.o help.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: chef saladmaker

chef: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

saladmaker: $(OBJ1)
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY : clean
clean:
	-rm -f $(OBJ) chef $(OBJ1) saladmaker valgrind.txt chef.txt pepper_log.txt onion_log.txt tomato_log.txt

run: chef
	./chef