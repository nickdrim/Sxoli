CC=gcc
DEPS = hash.h help.h list.h prompt.h student.h
OBJ = mngstd.o help.o hash.o insert.o number.o count.o lookup.o average.o minimum.o top.o studentlist.o delete.o exit.o prompt.o postalcode.o postlist.o bucketlist.o student.o yearlist.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< -g

mngstd: $(OBJ)
	$(CC) -o $@ $^ -g -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) mngstd

run: mngstd
	./mngstd