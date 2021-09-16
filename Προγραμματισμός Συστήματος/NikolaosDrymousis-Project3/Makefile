CC=gcc
CFLAGS = -Wall -g -pthread
DEPS = hash.h list.h header.h country_hash.h citizen_hash.h pid_hash.h visited.h cyclic_buffer.h
OBJ = monitorServer.o list.o bloom_filter.o visited.o citizen_hash.o hash.o skip_list.o help.o
OBJ2 = travelMonitorClient.o bloom_filter.o pid_hash.o country_hash.o help.o skip_list.o hash.o list.o citizen_hash.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: monitorServer travelMonitorClient

monitorServer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

travelMonitorClient: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS) -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) monitorServer $(OBJ2) travelMonitorClient $(wildcard log_file*)
