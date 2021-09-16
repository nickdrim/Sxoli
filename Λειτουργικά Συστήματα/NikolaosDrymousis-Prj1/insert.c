#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prompt.h"

//Implementation of insert
void insert(HTable records, YNode *head, pcode *start, char** arr){

	if(hashSearch(records, atoi(arr[1])) != NULL){
		printf("Student %d exists\n", atoi(arr[1]));
		return;
	}

	//Create and initialize new student
	pstudent temp = newStudent();
	temp->StudentID = atoi(arr[1]);
	temp->lastName = malloc(strlen(arr[2]) + 1);
	strcpy(temp->lastName, arr[2]);
	temp->firstName = malloc(strlen(arr[3]) + 1);
	strcpy(temp->firstName, arr[3]);
	temp->zip = atoi(arr[4]);
	temp->entryYear = atoi(arr[5]);
	temp->gpa = atof(arr[6]);

	//Create node for insertion to structures
	SNode new = newStudentNode(temp);

	//Insert function of structures are called
	yearListInsert(head, new);
	postListInsert(start, new);
	hashInsert(records, new);

	printf("Student %d inserted\n", temp->StudentID);

}