#include <stdio.h>
#include <stdlib.h>

void patientInformationLookup() {
    printf("\n[Patient Information Lookup] - Coming Soon!\n");
}

void doctorAppointment() {
    printf("\n[Doctor Appointment] - Coming Soon!\n");
}

void medicineInventory() {
    printf("\n[Medicine Inventory] - Coming Soon!\n");
}

void medicalReports() {
    printf("\n[Medical Reports] - Coming Soon!\n");
}

void emergencyPatientQueue() {
    printf("\n[Emergency Patient Queue] - Coming Soon!\n");
}

void clearScreen()
 {
    
    system("cls");
    
}

int main() {
    int choice;

    printf("\nWelcome to my project: Smart Medical Record Management System (SMRMS) \n");

    while (1) {
        printf("\n1. Patient Information Lookup\n");
        printf("2. Doctor Appointment\n");
        printf("3. Medicine Inventory\n");
        printf("4. Medical Reports\n");
        printf("5. Emergency Patient Queue\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");
        
        scanf("%d", &choice);

        clearScreen();

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
                printf("Invalid choice. Please enter a number between 1 and 6.\n");
        }

    }

    return 0;
} 