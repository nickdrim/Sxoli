#ifndef PROMPT_H
#define PROMPT_H

#include "help.h"
#include "hash.h"
#include "student.h"

void prompt(FILE *fp_inp, FILE *fp_conf);
void insert(HTable records, YNode *head, pcode *start, char** arr);
void delete(HTable records, YNode *head, pcode *start, int ID);
void number(YNode head, int date);
void count(YNode head);
void lookup(HTable records, int ID);
void average(YNode head, int date);
void minimum(YNode head, int date);
void top(YNode head, int number, int date);
void postalcode(pcode head, int rank);
void programmExit(YNode head, pcode start, HTable records);

int leastValue(SNode *arr, int number);

#endif