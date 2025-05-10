#include <stdio.h>
#include <stdlib.h>
#include "patient.h"

void doctorAppointment();
void medicineInventory();
void medicalReports();
void emergencyPatientQueue();

int main() {
    int choice;

    while (1) {
        system("cls");
        printf("==== ==== Welcome to Smart Medical Record Management System ==== ==== \n\n");
        printf("1. Patient Information Lookup\n");
        printf("2. Doctor Appointment\n");
        printf("3. Medicine Inventory\n");
        printf("4. Medical Reports\n");
        printf("5. Emergency Patient Queue\n");
        printf("6. Exit\n");
        printf("\nChoice: ");
        fflush(stdout);  // Ensure the prompt is displayed

        if (scanf("%d", &choice) != 1) {
            // Handle invalid input
            while (getchar() != '\n') {}
            printf("Invalid input. Press Enter to continue...");
            getchar();
            continue;
        }

        // Clear input buffer after successful read
        while (getchar() != '\n') {}

        switch (choice) {
            case 1:
                patientInformationLookup();
                break;
            case 2:
                doctorAppointment();
                break;
            case 3:
                medicineInventory();
                break;
            case 4:
                medicalReports();
                break;
            case 5:
                emergencyPatientQueue();
                break;
            case 6:
                printf("Exiting SMRMS... Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please enter 1-6.\n");
                printf("Press Enter to continue...");
                getchar();
        }
    }
}



void doctorAppointment() {
    printf("\n[Doctor Appointment] - Coming Soon!\n");
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}

void medicineInventory() {
    printf("\n[Medicine Inventory] - Coming Soon!\n");
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}

void medicalReports() {
    printf("\n[Medical Reports] - Coming Soon!\n");
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}

void emergencyPatientQueue(){
    printf("\n[Emergency Patient Queue] - Coming Soon!\n");
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}