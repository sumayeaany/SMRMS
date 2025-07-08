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
Patient findPatientBySearch(int searchType, const char* value1, const char* value2);
void makePatientEntry(Patient* patient);
void showPatient(Patient* patient);
void listAllPatients();
void editPatient(const int patientId, Patient* patient);
void deletePatient(const int patientId);
void searchPatient();
int generatePatientId();
void initializeMaxPatientId();
Patient findPatientByNameAndPhone(const char* name, const char* phone);
int searchAndShowPatientsByName(const char* name);

#endif