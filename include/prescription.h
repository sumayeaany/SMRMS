//
// Created by User on 09-Jul-25.
//

#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H
typedef struct {
    int prescriptionId;
    int patientId;
    int medicineId;
    char medicineName[50];
    int quantity;
    float unitPrice;
    float totalPrice;
    char prescribedDate[20];
    char prescribedBy[50];
    char dosage[100];
    char duration[50];
    char notes[200];
} Prescription;

// Function prototypes
void initializeMaxPrescriptionId();
int generatePrescriptionId();
void savePrescription(Prescription* prescription);
void addPrescription();
void viewPatientPrescriptions();
void viewAllPrescriptions();
void deletePrescription(int prescriptionId);
void prescriptionManagement();
Prescription findPrescriptionById(int prescriptionId);
void editPrescription(int prescriptionId);
void searchPrescriptionByPatient(int patientId);
#endif //PRESCRIPTION_H
