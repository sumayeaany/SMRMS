//
// Created by User on 09-Jul-25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "prescription.h"
#include "medicine.h"

#define PRESCRIPTION_DATAFILE "data/prescription.csv"

static int maxPrescriptionId = 3000;
static int isMaxPrescriptionIdInitialized = 0;

// Helper function to check if a string is effectively empty
static int isPrescriptionEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function for input
static void getPrescriptionInput(const char* prompt, char* dest, const size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)size, stdin)) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        dest[0] = '\0';
    }
}

// Helper: set "N/A" if input is empty
static void setPrescriptionOrNA(char* dest, const char* input, const size_t size) {
    if (!isPrescriptionEffectivelyEmpty(input))
        strncpy(dest, input, size - 1);
    else
        strncpy(dest, "N/A", size - 1);
    dest[size - 1] = '\0';
}

// Create data directory if it doesn't exist
static void createDataDirectory() {
    #ifdef _WIN32
        system("if not exist data mkdir data");
    #else
        system("mkdir -p data");
    #endif
}

void initializeMaxPrescriptionId() {
    if (isMaxPrescriptionIdInitialized) return;

    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (!fp) {
        maxPrescriptionId = 3000;
        isMaxPrescriptionIdInitialized = 1;
        return;
    }

    Prescription prescription;
    maxPrescriptionId = 3000;
    while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                  &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                  prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                  &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                  prescription.dosage, prescription.duration, prescription.notes) == 12) {
        if (prescription.prescriptionId > maxPrescriptionId) {
            maxPrescriptionId = prescription.prescriptionId;
        }
    }
    fclose(fp);
    isMaxPrescriptionIdInitialized = 1;
}

int generatePrescriptionId() {
    if (!isMaxPrescriptionIdInitialized) {
        initializeMaxPrescriptionId();
    }
    return ++maxPrescriptionId;
}

void savePrescription(Prescription* prescription) {
    createDataDirectory();

    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "a");
    if (!fp) {
        printf("Error saving prescription.\n");
        return;
    }

    fprintf(fp, "%d,%d,%d,%s,%d,%.2f,%.2f,%s,%s,%s,%s,%s\n",
            prescription->prescriptionId, prescription->patientId, prescription->medicineId,
            prescription->medicineName, prescription->quantity, prescription->unitPrice,
            prescription->totalPrice, prescription->prescribedDate, prescription->prescribedBy,
            prescription->dosage, prescription->duration, prescription->notes);

    fclose(fp);
}

void addPrescription() {
    char buffer[256];
    int patientId = 0;
    char prescribedBy[50] = "";
    int firstPrescription = 1;

    printf("\n==== Add New Prescription ====\n");

    // Get patient ID once for the session
    getPrescriptionInput("Patient ID: ", buffer, sizeof(buffer));
    patientId = atoi(buffer);
    if (patientId <= 0) {
        printf("Invalid Patient ID!\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    // Get prescribing doctor once for the session
    getPrescriptionInput("Prescribed by (Doctor name): ", buffer, sizeof(buffer));
    setPrescriptionOrNA(prescribedBy, buffer, sizeof(prescribedBy));

    char addMore;
    do {
        Prescription prescription = {0};
        prescription.prescriptionId = generatePrescriptionId();
        prescription.patientId = patientId;
        strcpy(prescription.prescribedBy, prescribedBy);

        if (firstPrescription) {
            printf("\n--- Medicine #1 ---\n");
            firstPrescription = 0;
        } else {
            printf("\n--- Additional Medicine ---\n");
        }

        // Medicine ID
        getPrescriptionInput("Medicine ID: ", buffer, sizeof(buffer));
        prescription.medicineId = atoi(buffer);
        if (prescription.medicineId <= 0) {
            printf("Invalid Medicine ID! Skipping this medicine.\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }

        // Get medicine details
        const Medicine medicine = findMedicine(prescription.medicineId);
        if (medicine.medicineId == 0) {
            printf("Medicine not found! Skipping this medicine.\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }

        strcpy(prescription.medicineName, medicine.name);
        prescription.unitPrice = medicine.price;

        // Quantity
        getPrescriptionInput("Quantity: ", buffer, sizeof(buffer));
        prescription.quantity = atoi(buffer);
        if (prescription.quantity <= 0) {
            printf("Invalid quantity! Skipping this medicine.\n");
            printf("Press Enter to continue...");
            getchar();
            continue;
        }

        prescription.totalPrice = prescription.quantity * prescription.unitPrice;

        // Dosage
        getPrescriptionInput("Dosage: ", buffer, sizeof(buffer));
        setPrescriptionOrNA(prescription.dosage, buffer, sizeof(prescription.dosage));

        // Duration
        getPrescriptionInput("Duration: ", buffer, sizeof(buffer));
        setPrescriptionOrNA(prescription.duration, buffer, sizeof(prescription.duration));

        // Notes
        getPrescriptionInput("Notes: ", buffer, sizeof(buffer));
        setPrescriptionOrNA(prescription.notes, buffer, sizeof(prescription.notes));

        // Set current date
        time_t now;
        time(&now);
        const struct tm *timeinfo = localtime(&now);
        strftime(prescription.prescribedDate, sizeof(prescription.prescribedDate), "%d/%m/%Y", timeinfo);

        savePrescription(&prescription);

        printf("Medicine prescribed successfully!\n");
        printf("Prescription ID: %d\n", prescription.prescriptionId);
        printf("Medicine: %s\n", prescription.medicineName);
        printf("Quantity: %d\n", prescription.quantity);
        printf("Total cost: Tk.%.2f\n", prescription.totalPrice);

        // Ask if user wants to add another medicine
        do {
            printf("\nAdd another medicine for this patient? (y/n): ");
            scanf(" %c", &addMore);
            getchar(); // consume newline
            addMore = tolower(addMore);
        } while (addMore != 'y' && addMore != 'n');

    } while (addMore == 'y');

    printf("\nPrescription session completed for Patient ID: %d\n", patientId);
    printf("Press Enter to return to menu...");
    getchar();
}
void viewPatientPrescriptions() {
    char buffer[20];
    getPrescriptionInput("Enter Patient ID: ", buffer, sizeof(buffer));
    const int patientId = atoi(buffer);

    if (patientId <= 0) {
        printf("Invalid Patient ID!\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (!fp) {
        printf("No prescription data found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    printf("\n==== Prescriptions for Patient ID: %d ====\n", patientId);
    printf("%-5s %-20s %-4s %-10s %-10s %-12s %-15s\n",
           "ID", "Medicine", "Qty", "Unit Price", "Total", "Date", "Prescribed By");
    printf("--------------------------------------------------------------------------------\n");

    Prescription prescription;
    int found = 0;

    while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                  &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                  prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                  &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                  prescription.dosage, prescription.duration, prescription.notes) == 12) {

        if (prescription.patientId == patientId) {
            printf("%-5d %-20s %-4d Tk.%-9.2f Tk.%-9.2f %-12s %-15s\n",
                   prescription.prescriptionId, prescription.medicineName, prescription.quantity,
                   prescription.unitPrice, prescription.totalPrice, prescription.prescribedDate,
                   prescription.prescribedBy);
            found = 1;
        }
    }

    if (!found) {
        printf("No prescriptions found for this patient.\n");
    }

    fclose(fp);
    printf("Press Enter to return to menu...");
    getchar();
}

void viewAllPrescriptions() {
    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (!fp) {
        printf("No prescription data found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    printf("\n==== All Prescriptions ====\n");
    printf("%-5s %-10s %-20s %-4s %-10s %-10s %-12s\n",
           "ID", "Patient", "Medicine", "Qty", "Unit Price", "Total", "Date");
    printf("--------------------------------------------------------------------------------\n");

    Prescription prescription;
    while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                  &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                  prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                  &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                  prescription.dosage, prescription.duration, prescription.notes) == 12) {

        printf("%-5d %-10d %-20s %-4d Tk.%-9.2f Tk.%-9.2f %-12s\n",
               prescription.prescriptionId, prescription.patientId, prescription.medicineName,
               prescription.quantity, prescription.unitPrice, prescription.totalPrice,
               prescription.prescribedDate);
    }

    fclose(fp);
    printf("Press Enter to return to menu...");
    getchar();
}

Prescription findPrescriptionById(const int prescriptionId) {
    Prescription prescription = {0};
    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (!fp) {
        return prescription;
    }

    while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                  &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                  prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                  &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                  prescription.dosage, prescription.duration, prescription.notes) == 12) {

        if (prescription.prescriptionId == prescriptionId) {
            fclose(fp);
            return prescription;
        }
    }

    fclose(fp);
    prescription.prescriptionId = 0; // Not found
    return prescription;
}

void editPrescription(const int prescriptionId) {
    Prescription prescription = findPrescriptionById(prescriptionId);
    if (prescription.prescriptionId == 0) {
        printf("Prescription with ID %d not found.\n", prescriptionId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    char buffer[256];
    char prompt[256];

    printf("\n==== Editing Prescription (ID: %d) ====\n", prescription.prescriptionId);
    printf("For each field, press Enter to keep current value or enter new data\n\n");

    // Patient ID
    snprintf(prompt, sizeof(prompt), "Patient ID [%d]: ", prescription.patientId);
    getPrescriptionInput(prompt, buffer, sizeof(buffer));
    if (!isPrescriptionEffectivelyEmpty(buffer)) {
        prescription.patientId = atoi(buffer);
    }

    // Medicine ID
    snprintf(prompt, sizeof(prompt), "Medicine ID [%d]: ", prescription.medicineId);
    getPrescriptionInput(prompt, buffer, sizeof(buffer));
    if (!isPrescriptionEffectivelyEmpty(buffer)) {
        int newMedicineId = atoi(buffer);
        const Medicine medicine = findMedicine(newMedicineId);
        if (medicine.medicineId != 0) {
            prescription.medicineId = newMedicineId;
            strcpy(prescription.medicineName, medicine.name);
            prescription.unitPrice = medicine.price;
            prescription.totalPrice = prescription.quantity * prescription.unitPrice;
        } else {
            printf("Medicine not found, keeping original medicine.\n");
        }
    }

    // Quantity
    snprintf(prompt, sizeof(prompt), "Quantity [%d]: ", prescription.quantity);
    getPrescriptionInput(prompt, buffer, sizeof(buffer));
    if (!isPrescriptionEffectivelyEmpty(buffer)) {
        prescription.quantity = atoi(buffer);
        prescription.totalPrice = prescription.quantity * prescription.unitPrice;
    }

    // Prescribed by
    snprintf(prompt, sizeof(prompt), "Prescribed by [%s]: ", prescription.prescribedBy);
    getPrescriptionInput(prompt, buffer, sizeof(buffer));
    if (!isPrescriptionEffectivelyEmpty(buffer)) {
        setPrescriptionOrNA(prescription.prescribedBy, buffer, sizeof(prescription.prescribedBy));
    }

    // Dosage
    snprintf(prompt, sizeof(prompt), "Dosage [%s]: ", prescription.dosage);
    getPrescriptionInput(prompt, buffer, sizeof(buffer));
    if (!isPrescriptionEffectivelyEmpty(buffer)) {
        setPrescriptionOrNA(prescription.dosage, buffer, sizeof(prescription.dosage));
    }

    // Duration
    snprintf(prompt, sizeof(prompt), "Duration [%s]: ", prescription.duration);
    getPrescriptionInput(prompt, buffer, sizeof(buffer));
    if (!isPrescriptionEffectivelyEmpty(buffer)) {
        setPrescriptionOrNA(prescription.duration, buffer, sizeof(prescription.duration));
    }

    // Notes
    snprintf(prompt, sizeof(prompt), "Notes [%s]: ", prescription.notes);
    getPrescriptionInput(prompt, buffer, sizeof(buffer));
    if (!isPrescriptionEffectivelyEmpty(buffer)) {
        setPrescriptionOrNA(prescription.notes, buffer, sizeof(prescription.notes));
    }

    // Update file
    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
    FILE *temp = fopen("data/temp_prescription.csv", "w");

    if (!fp || !temp) {
        printf("Error accessing files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Prescription currentPrescription;
    int found = 0;

    while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                  &currentPrescription.prescriptionId, &currentPrescription.patientId, &currentPrescription.medicineId,
                  currentPrescription.medicineName, &currentPrescription.quantity, &currentPrescription.unitPrice,
                  &currentPrescription.totalPrice, currentPrescription.prescribedDate, currentPrescription.prescribedBy,
                  currentPrescription.dosage, currentPrescription.duration, currentPrescription.notes) == 12) {

        if (currentPrescription.prescriptionId == prescriptionId) {
            found = 1;
            fprintf(temp, "%d,%d,%d,%s,%d,%.2f,%.2f,%s,%s,%s,%s,%s\n",
                    prescription.prescriptionId, prescription.patientId, prescription.medicineId,
                    prescription.medicineName, prescription.quantity, prescription.unitPrice,
                    prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                    prescription.dosage, prescription.duration, prescription.notes);
        } else {
            fprintf(temp, "%d,%d,%d,%s,%d,%.2f,%.2f,%s,%s,%s,%s,%s\n",
                    currentPrescription.prescriptionId, currentPrescription.patientId, currentPrescription.medicineId,
                    currentPrescription.medicineName, currentPrescription.quantity, currentPrescription.unitPrice,
                    currentPrescription.totalPrice, currentPrescription.prescribedDate, currentPrescription.prescribedBy,
                    currentPrescription.dosage, currentPrescription.duration, currentPrescription.notes);
        }
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove(PRESCRIPTION_DATAFILE);
        rename("data/temp_prescription.csv", PRESCRIPTION_DATAFILE);
        printf("Prescription updated successfully.\n");
    } else {
        remove("data/temp_prescription.csv");
        printf("Error updating prescription.\n");
    }

    printf("Press Enter to return to menu...");
    getchar();
}

void deletePrescription(const int prescriptionId) {
    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (!fp) {
        printf("No prescription data found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    FILE *temp = fopen("data/temp_prescription.csv", "w");
    if (!temp) {
        printf("Error creating temporary file.\n");
        fclose(fp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Prescription prescription;
    int found = 0;

    while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                  &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                  prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                  &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                  prescription.dosage, prescription.duration, prescription.notes) == 12) {

        if (prescription.prescriptionId != prescriptionId) {
            fprintf(temp, "%d,%d,%d,%s,%d,%.2f,%.2f,%s,%s,%s,%s,%s\n",
                    prescription.prescriptionId, prescription.patientId, prescription.medicineId,
                    prescription.medicineName, prescription.quantity, prescription.unitPrice,
                    prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                    prescription.dosage, prescription.duration, prescription.notes);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove(PRESCRIPTION_DATAFILE);
        rename("data/temp_prescription.csv", PRESCRIPTION_DATAFILE);
        printf("Prescription deleted successfully.\n");
    } else {
        remove("data/temp_prescription.csv");
        printf("Prescription with ID %d not found.\n", prescriptionId);
    }

    printf("Press Enter to return to menu...");
    getchar();
}

void searchPrescriptionByPatient(const int patientId) {
    FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (!fp) {
        printf("No prescription data found.\n");
        return;
    }

    printf("\n==== Prescriptions for Patient ID: %d ====\n", patientId);
    printf("%-5s %-20s %-4s %-10s %-10s %-12s\n",
           "ID", "Medicine", "Qty", "Unit Price", "Total", "Date");
    printf("----------------------------------------------------------------\n");

    Prescription prescription;
    int found = 0;

    while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                  &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                  prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                  &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                  prescription.dosage, prescription.duration, prescription.notes) == 12) {

        if (prescription.patientId == patientId) {
            printf("%-5d %-20s %-4d $%-9.2f $%-9.2f %-12s\n",
                   prescription.prescriptionId, prescription.medicineName, prescription.quantity,
                   prescription.unitPrice, prescription.totalPrice, prescription.prescribedDate);
            found = 1;
        }
    }

    if (!found) {
        printf("No prescriptions found for this patient.\n");
    }

    fclose(fp);
}

void prescriptionManagement() {
    initializeMaxPrescriptionId();
    int choice;
    char buffer[20];

    while (1) {
        system("cls");
        printf("==== Prescription Management ====\n\n");
        printf("1. Add Prescription\n");
        printf("2. View Patient Prescriptions\n");
        printf("3. View All Prescriptions\n");
        printf("4. Edit Prescription\n");
        printf("5. Delete Prescription\n");
        printf("6. Search Prescription by ID\n");
        printf("7. Back to Main Menu\n");
        printf("\nChoice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n') {}
            printf("Invalid input. Press Enter to continue...");
            getchar();
            continue;
        }
        getchar();

        switch (choice) {
            case 1:
                addPrescription();
                break;
            case 2:
                viewPatientPrescriptions();
                break;
            case 3:
                viewAllPrescriptions();
                break;
            case 4: {
                getPrescriptionInput("Enter Prescription ID to edit: ", buffer, sizeof(buffer));
                int prescriptionId = atoi(buffer);
                if (prescriptionId > 0) {
                    editPrescription(prescriptionId);
                } else {
                    printf("Invalid Prescription ID!\n");
                    printf("Press Enter to continue...");
                    getchar();
                }
                break;
            }
            case 5: {
                getPrescriptionInput("Enter Prescription ID to delete: ", buffer, sizeof(buffer));
                int prescriptionId = atoi(buffer);
                if (prescriptionId > 0) {
                    deletePrescription(prescriptionId);
                } else {
                    printf("Invalid Prescription ID!\n");
                    printf("Press Enter to continue...");
                    getchar();
                }
                break;
            }
            case 6: {
                getPrescriptionInput("Enter Prescription ID to search: ", buffer, sizeof(buffer));
                int prescriptionId = atoi(buffer);
                if (prescriptionId > 0) {
                    Prescription prescription = findPrescriptionById(prescriptionId);
                    if (prescription.prescriptionId != 0) {
                        printf("\n==== Prescription Details ====\n");
                        printf("Prescription ID: %d\n", prescription.prescriptionId);
                        printf("Patient ID: %d\n", prescription.patientId);
                        printf("Medicine: %s (ID: %d)\n", prescription.medicineName, prescription.medicineId);
                        printf("Quantity: %d\n", prescription.quantity);
                        printf("Unit Price: $%.2f\n", prescription.unitPrice);
                        printf("Total Price: $%.2f\n", prescription.totalPrice);
                        printf("Prescribed Date: %s\n", prescription.prescribedDate);
                        printf("Prescribed By: %s\n", prescription.prescribedBy);
                        printf("Dosage: %s\n", prescription.dosage);
                        printf("Duration: %s\n", prescription.duration);
                        printf("Notes: %s\n", prescription.notes);
                    } else {
                        printf("Prescription not found!\n");
                    }
                    printf("Press Enter to continue...");
                    getchar();
                } else {
                    printf("Invalid Prescription ID!\n");
                    printf("Press Enter to continue...");
                    getchar();
                }
                break;
            }
            case 7:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}