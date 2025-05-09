#ifndef PATIENT_H
#define PATIENT_H

// Patient structure with expanded information
typedef struct {
    int patientId;          // Unique identifier
    char name[50];
    int age;
    char gender;            // 'M', 'F', or 'O'
    char phone[15];
    char address[100];
    char email[50];
    char bloodType[5];      // A+, B-, O+, etc.
    char allergies[200];    // Known allergies// Insurance provider
    char emergencyContact[100]; // Emergency contact info
    char primaryDoctor[50]; // Primary care physician
} Patient;

// Function declarations
void initializeMaxPatientId();
int generatePatientId();
Patient makePatient(const char*, const char*);
Patient findPatient(int, const char*);
void show(Patient*);
void makeEntry(Patient*);
void listAllPatients();
void editPatient(char*);
void deletePatient(char*);
void patientInformationLookup();

#endif // PATIENT_H