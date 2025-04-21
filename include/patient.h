#ifndef PATIENT_H
#define PATIENT_H

#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 12  

typedef struct {
    char name[MAX_NAME_LEN];
    int age;
    char number[MAX_PHONE_LEN];
} patient;

// Checks if a patient with given name or phone exists in the CSV file
// Returns 1 if found, 0 otherwise
int exists(char name[], char phn[]);

// Shows patient details for the given phone number
void show(char phn[]);

// Adds a new patient record to the CSV file if it doesn't already exist
void makeEntry(char name[], int age, char phn[]);

#endif // PATIENT_H