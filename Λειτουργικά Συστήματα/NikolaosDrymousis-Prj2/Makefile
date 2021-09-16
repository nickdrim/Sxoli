CC=gcc
DEPS = header.h list.h help.h
OBJ = myprime.o calculate.o list.o help.o
OBJ1 = prime1.o
OBJ2 = prime2.o
OBJ3 = prime3.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< -g -Wall

all: myprime prime1 prime2 prime3

myprime: $(OBJ)
	$(CC) -o $@ $^ -g -lm

prime1: $(OBJ1)
	$(CC) -o $@ $^ -g -lm

prime2: $(OBJ2)
	$(CC) -o $@ $^ -g -lm

prime3: $(OBJ3)
	$(CC) -o $@ $^ -g -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) myprime $(OBJ1) prime1 $(OBJ2) prime2 $(OBJ3) prime3 $(wildcard process*)

run: myprime
	./myprime 