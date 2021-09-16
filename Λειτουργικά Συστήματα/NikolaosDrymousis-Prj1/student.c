#include <stdio.h>
#include <stdlib.h>
#include "student.h"

//Creates and returns new student
pstudent newStudent(void){
	pstudent temp = malloc(sizeof(student));

	temp->StudentID = -1;
	temp->firstName = NULL;
	temp->lastName = NULL;
	temp->zip = -1;
	temp->entryYear = -1;
	temp->gpa = -1;

	return temp;
}

//Frees any space allocate by student struct
void deleteStudent(pstudent temp){
	free(temp->firstName);
	free(temp->lastName);
	free(temp);
}