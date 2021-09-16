CC=gcc
CFLAGS = -Wall -g
DEPS = hash.h list.h header.h country_hash.h citizen_hash.h population.h
OBJ = vaccineMonitor.o list.o bloom_filter.o population.o citizen_hash.o hash.o skip_list.o command_line.o command_line_functions.o country_skip_list.o country_hash.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

vaccineMonitor: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) vaccineMonitor
