#ifndef HELP_H
#define HELP_H

void command_guide(void);
int isString(char *word);
int validInt(char *word);
int validFloat(const char *str, float *number);
int compareString(char *firstString, char* secondString);
void find_max(double *arr, int size);
void find_min(double *arr, int size);
void time_of_leafs(double *arr, int size);
int count_digit(int number);

#endif