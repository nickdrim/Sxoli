CC=gcc
CFLAGS = -Wall -g
DEPS = hash.h list.h header.h country_hash.h citizen_hash.h pid_hash.h visited.h
OBJ = Monitor.o list.o bloom_filter.o visited.o citizen_hash.o hash.o skip_list.o help.o
OBJ2 = travelMonitor.o bloom_filter.o pid_hash.o country_hash.o help.o skip_list.o hash.o list.o citizen_hash.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: Monitor travelMonitor

Monitor: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

travelMonitor: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS) -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) Monitor $(OBJ2) travelMonitor $(wildcard log_file*)
