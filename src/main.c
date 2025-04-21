#include <stdio.h>
#include <stdlib.h>
#include "patient.h"

void patientInformationLookup();
void doctorAppointment();
void medicineInventory();
void medicalReports();
void emergencyPatientQueue();

int main() {
    int choice;

    while (1) {
        system("cls");  
        printf("\nWelcome to SMRMS\n");
        printf("1. Patient Information Lookup\n");
        printf("2. Doctor Appointment\n");
        printf("3. Medicine Inventory\n");
        printf("4. Medical Reports\n");
        printf("5. Emergency Patient Queue\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

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
                return 0;
            default:
                printf("Invalid choice. Please enter 1-6.\n");
                printf("Press Enter to continue...");
                getchar(); getchar();
        }
    }

    return 0;
}

void patientInformationLookup() {
    int choice;
    char name[50];
    int phone, age;

    printf("\n1. Search Patient\n2. Back\n\nChoice: ");
    scanf("%d", &choice);

    if (choice == 2) {
        return; 
    }

    printf("Name: ");
    scanf("%s", name);
    printf("Number: ");
    scanf("%d", &phone);

    if (exists(name, phone)) {
        show(phone);
    } else {
        printf("No entry for this patient!\nAdd one? (Y/n): ");
        char c;
        scanf(" %c", &c); 
        if (c == 'y' || c == 'Y') {
            printf("Name: ");
            scanf("%s", name);
            printf("Age: ");
            scanf("%d", &age);
            printf("Number: ");
            scanf("%d", &phone);
            makeEntry(name, age, phone);
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

void emergencyPatientQueue() {
    printf("\n[Emergency Patient Queue] - Coming Soon!\n");
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}