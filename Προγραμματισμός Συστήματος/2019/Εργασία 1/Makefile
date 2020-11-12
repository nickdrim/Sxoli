CC=gcc
DEPS = hashTable.h help.h binary_tree.h linked_list.h record.h
OBJ = diseaseMonitor.o linkList.o help.o hashFunctions.o BTFunctions.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< -g

diseaseMonitor: $(OBJ)
	$(CC) -o $@ $^ -g -lm

.PHONY : clean
clean:
	-rm -f $(OBJ) diseaseMonitor

run: diseaseMonitor
	./diseaseMonitor