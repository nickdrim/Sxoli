#ifndef STUDENT_H
#define STUDENT_H

typedef struct person{
	int StudentID;
	char* firstName;
	char* lastName;
	int zip;
	int	entryYear;
	float gpa;
}student,*pstudent;

pstudent newStudent(void);
void deleteStudent(pstudent temp);

#endif