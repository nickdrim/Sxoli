CC=gcc
CFLAGS = -Wall -g -lm
DEPS = quic.h structs.h
OBJ = quic.o copy_functions.o hash_functions.o list_functions.o tree_functions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

quic: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY : clean
clean:
	-rm -f $(OBJ) quic

run: quic
	./quic