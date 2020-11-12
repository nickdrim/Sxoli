#ifndef RECORDS_H
#define RECORDS_H

typedef struct info{
	int recordID;
	char* patientFirstName;
	char* patientLastName;
	char* diseaseID;
	char* country;
	char* entryDate;
	char* exitDate;
}records,*rpointer;


#endif