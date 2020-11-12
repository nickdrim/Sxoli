CC=gcc
DEPS = hash.h list.h header.h
OBJ = main.o implementation.o hash.o cliquelist.o recordlist.o hashlist.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< -g

project1: $(OBJ)
	$(CC) -o $@ $^ -g -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) project1

run: project1
	./project1