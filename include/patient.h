#ifndef PATIENT_H
#define PATIENT_H

typedef struct {
    int patientId;
    char name[50];
    int age;
    char gender;
    char phone[15];
    char address[100];
    char email[50];
    char bloodType[5];
    char allergies[200];
    char emergencyContact[15];
    char primaryDoctor[50];
} Patient;

// Function declarations
void patientInformationLookup();
Patient makePatient();
void makePatientEntry(Patient* patient);
Patient findPatient(const int searchType, const char* searchValue);
void showPatient(Patient* patient);
void listAllPatients();
void editPatient(const int patientId, Patient* patient);
void deletePatient(const int patientId);
void searchPatient();
int generatePatientId();
void initializeMaxPatientId();

#endif