#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "patient.h"

#define DATAFILE "data/patient.csv"
// Static variable to store the maximum patient ID
static int maxPatientId = 1000;
static int isMaxIdInitialized = 0;

// Initialize the maximum patient ID once
void initializeMaxPatientId() {
    if (isMaxIdInitialized) return;

    FILE *fp = fopen(DATAFILE, "r");
    if (!fp) {
        maxPatientId = 1000; // Start from 1001 for the first patient
        isMaxIdInitialized = 1;
        return;
    }

    char line[512];

    // Skip header line
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        int id = atoi(strtok(line, ","));
        if (id > maxPatientId) maxPatientId = id;
    }

    fclose(fp);
    isMaxIdInitialized = 1;
}

// Modified generatePatientId function
int generatePatientId() {
    // Make sure maxPatientId is initialized
    if (!isMaxIdInitialized) {
        initializeMaxPatientId();
    }

    // Increment and return the next ID
    return ++maxPatientId;
}
// Generate a unique patient ID

Patient findPatient(const int searchType, const char *searchValue) {
    FILE *fp = fopen(DATAFILE, "r");
    if (!fp) {
        perror("Unable to open data file");
        exit(EXIT_FAILURE);
    }

    char line[512];
    int found = 0;
    Patient patient = {0};

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        const char* tok_id = strtok(lineCopy, ",");
        const char* tok_name = strtok(NULL, ",");
        const char* tok_age = strtok(NULL, ",");
        const char* tok_gender = strtok(NULL, ",");
        const char* tok_phone = strtok(NULL, ",");
        const char* tok_address = strtok(NULL, ",");
        const char* tok_email = strtok(NULL, ",");
        const char* tok_blood = strtok(NULL, ",");
        const char* tok_allergies = strtok(NULL, ",");
        const char* tok_emergency = strtok(NULL, ",");
        const char* tok_doctor = strtok(NULL, ",\n");

        if (!tok_id || !tok_name || !tok_phone) continue;

        int id = atoi(tok_id);


        switch(searchType) {
            case 1: // Phone
                if (strcmp(tok_phone, searchValue) == 0) {
                    found = 1;
                }
                break;
            case 2: // Name
                {
                    char nameLower[50], searchLower[50];
                    strcpy(nameLower, tok_name);
                    strcpy(searchLower, searchValue);

                    for (int i = 0; nameLower[i]; i++)
                        nameLower[i] = tolower((int)nameLower[i]);
                    for (int i = 0; searchLower[i]; i++)
                        searchLower[i] = tolower((int)searchLower[i]);

                    if (strstr(nameLower, searchLower)) {
                        found = 1;
                    }
                }
                break;
            case 3: // ID
                if (id == atoi(searchValue)) {
                    found = 1;
                }
                break;
            default:
                printf("Invalid search type.\n");
        }

        if (found) {
            patient.patientId = atoi(tok_id);
            strcpy(patient.name, tok_name);
            patient.age = atoi(tok_age);
            patient.gender = tok_gender[0];
            strcpy(patient.phone, tok_phone);

            if (tok_address) strcpy(patient.address, tok_address);
            if (tok_email) strcpy(patient.email, tok_email);
            if (tok_blood) strcpy(patient.bloodType, tok_blood);
            if (tok_allergies) strcpy(patient.allergies, tok_allergies);
            if (tok_emergency) strcpy(patient.emergencyContact, tok_emergency);
            if (tok_doctor) strcpy(patient.primaryDoctor, tok_doctor);

            break; // Exit the loop once found
        }
    }
    fclose(fp);
    return patient;
}

// Helper function to check if a string is effectively empty (only whitespace)
static int isEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

void show(Patient* patient) {
    printf("\n==== Patient Information ====\n");
    printf("ID: %d\n", patient->patientId);
    printf("Name: %s\n", patient->name);
    printf("Age: %d\n", patient->age);
    printf("Gender: %s\n", patient->gender == 'M' ? "Male" : "Female");
    printf("Phone: %s\n", patient->phone);
    printf("Address: %s\n", !isEffectivelyEmpty(patient->address) ? patient->address : "N/A");
    printf("Email: %s\n", !isEffectivelyEmpty(patient->email) ? patient->email : "N/A");
    printf("Blood Type: %s\n", !isEffectivelyEmpty(patient->bloodType) ? patient->bloodType : "N/A");
    printf("Allergies: %s\n", !isEffectivelyEmpty(patient->allergies) ? patient->allergies : "N/A");
    printf("Emergency Contact: %s\n", !isEffectivelyEmpty(patient->emergencyContact) ? patient->emergencyContact : "N/A");
    printf("Primary Doctor: %s\n", !isEffectivelyEmpty(patient->primaryDoctor) ? patient->primaryDoctor : "N/A");

    printf("Press Enter to return to menu...");
    getchar();
}

void makeEntry(Patient *patient) {

    if (findPatient(3, patient->phone).patientId && findPatient(2, patient->name).patientId) {
        printf("A patient with this name or phone already exists.\n");
    } else {
        FILE *fp = fopen(DATAFILE, "a");
        if (!fp) {
            perror("Unable to open data file");
            return;
        }

        if (patient->patientId == 0) {
            patient->patientId = generatePatientId();
        }

        fprintf(fp, "%d,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s\n",
                patient->patientId,
                patient->name,
                patient->age,
                patient->gender,
                patient->phone,
                patient->address,
                patient->email,
                patient->bloodType,
                patient->allergies,
                patient->emergencyContact,
                patient->primaryDoctor);
        fclose(fp);
        printf("Patient added successfully with ID: %d\n", patient->patientId);
    }
    printf("Press Enter to return to menu...");
    getchar();
}

void listAllPatients() {
    FILE *fp = fopen(DATAFILE, "r");
    if (!fp) {
        perror("Unable to open data file");
        return;
    }

    char line[512];
    int count = 0;

    printf("\n==== All Patients ====\n");
    printf("%-8s %-12s %-5s %-10s %-15s %-12s\n", "ID", "Name", "Age", "Gender", "Phone", "Physician");
    printf("------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        char *tok_id = strtok(lineCopy, ",");
        char *tok_name = strtok(NULL, ",");
        char *tok_age = strtok(NULL, ",");
        char *tok_gender = strtok(NULL, ","); // gender
        char *tok_phn = strtok(NULL, ",");
        strtok(NULL, ","); // address
        strtok(NULL, ","); // email
        strtok(NULL, ","); // blood
        strtok(NULL, ","); // allergies
        strtok(NULL, ","); // emergency
        char *tok_doctor = strtok(NULL, ",\n");

        if (!tok_id || !tok_name) continue;

        printf("%-8s %-12s %-5s %-10s %-15s %-12s\n",
               tok_id, tok_name, tok_age, tok_gender[0] == 'M'? "Male" : "Female" , tok_phn,
               tok_doctor ? tok_doctor : "N/A");
        count++;
    }

    printf("\nTotal patients: %d\n", count);
    printf("Press Enter to return to menu...");
    getchar();

    fclose(fp);
}

Patient makePatient(const char* name, const char* phone) {
    Patient newPatient = {0}; // Initialize with zeros
    char buffer[256];

    printf("\n==== Add New Patient ====\n");
    if (name) {
        printf("Name: %s\n", name);
        strcpy(newPatient.name, name);
    }
    else {
        printf("Name: ");
        if (fgets(buffer, sizeof(buffer), stdin)) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) > 0)
                strncpy(newPatient.name, buffer, sizeof(newPatient.name) - 1);
        }
    }

    // FIXED: Add an explicit prompt and wait for input
    printf("Age: ");
    fflush(stdout);  // Force the prompt to display

    // Read input with careful handling
    memset(buffer, 0, sizeof(buffer));
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            newPatient.age = atoi(buffer);
        else
            newPatient.age = 0;
    } else {
        newPatient.age = 0;
    }

    printf("Gender (M/F): ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            newPatient.gender = buffer[0];
    }

    if (phone) {
        printf("Phone Number: %s\n", phone);
        strcpy(newPatient.phone, phone);
    }
    else {
        printf("Phone Number: ");
        if (fgets(buffer, sizeof(buffer), stdin)) {
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) > 0)
                strncpy(newPatient.phone, buffer, sizeof(newPatient.phone) - 1);
        }
    }

    printf("Address: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            strncpy(newPatient.address, buffer, sizeof(newPatient.address) - 1);
    }

    printf("Email: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            strncpy(newPatient.email, buffer, sizeof(newPatient.email) - 1);
    }

    printf("Blood Type: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            strncpy(newPatient.bloodType, buffer, sizeof(newPatient.bloodType) - 1);
    }

    printf("Allergies: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            strncpy(newPatient.allergies, buffer, sizeof(newPatient.allergies) - 1);
    }

    printf("Emergency Contact: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            strncpy(newPatient.emergencyContact, buffer, sizeof(newPatient.emergencyContact) - 1);
    }

    printf("Primary Doctor: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0)
            strncpy(newPatient.primaryDoctor, buffer, sizeof(newPatient.primaryDoctor) - 1);
    }

    return newPatient;
}

void editPatient(char phn[]) {
    // Implementation to edit patient details
    // This would involve reading all patients, modifying the target, and rewriting the file
    printf("Edit functionality not yet implemented.\n");
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}

void deletePatient(char phn[]) {
    // Implementation to delete a patient record
    printf("Delete functionality not yet implemented.\n");
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}

void patientInformationLookup() {
    int choice;
    while (1) {
        // Clear the screen first
        system("cls");

        // Print menu with buffer flushing
        printf("\n==== Patient Information Lookup ====\n");
        printf("1. Search by ID\n");
        printf("2. Search by Name\n");
        printf("3. Search by Phone Number\n");
        printf("4. View All Patients\n");
        printf("5. Add New Patient\n");
        printf("6. Edit Patient Information\n");
        printf("7. Delete Patient Record\n");
        printf("8. Return to Main Menu\n");
        printf("\nChoice: ");
        fflush(stdout);  // Force the menu to display

        // Get user choice
        if (scanf("%d", &choice) != 1) {
            // Handle invalid input
            while (getchar() != '\n') {}
            getchar();
            continue;
        }

        // Clear input buffer
        while (getchar() != '\n'){}

        switch (choice) {
            case 1: {
                char id[5];
                printf("\nEnter patient ID: ");
                scanf("%s", id);
                while (getchar() != '\n'){}
                Patient patient = findPatient(3, id);
                if (patient.patientId)
                    show(&patient);
                else {
                    printf("Patient not found!\n");
                    char c;
                    printf("Add Patient? (y/n): ");
                    scanf(" %c", &c);
                    if (c == 'y' || c == 'Y') {
                        while (getchar() != '\n'){}
                        Patient newPatient = makePatient(NULL, NULL);
                        makeEntry(&newPatient);
                    }
                }
                break;
            }
            case 2: {
                char name[50];
                printf("\nEnter name (full or partial): ");
                scanf("%s", name);
                while (getchar() != '\n'){}
                Patient patient = findPatient(2, name);
                if (patient.patientId)
                    show(&patient);
                else {
                    printf("Patient not found!\n");
                    char c;
                    printf("Add Patient? (y/n): ");
                    scanf(" %c", &c);
                    if (c == 'y' || c == 'Y') {
                        while (getchar() != '\n'){}
                        Patient newPatient = makePatient(name, NULL);
                        makeEntry(&newPatient);
                    }
                }
                break;
            }
            case 3: {
                char phone[15];
                printf("\nEnter phone number: ");
                scanf("%s", phone);
                while (getchar() != '\n'){}// Clear buffer
                Patient patient = findPatient(1, phone);
                if (patient.patientId)
                    show(&patient);
                else {
                    printf("Patient not found!\n");
                    char c;
                    printf("Add Patient? (y/n): ");
                    scanf(" %c", &c);
                    if (c == 'y' || c == 'Y') {
                        while (getchar() != '\n'){}
                        Patient newPatient = makePatient(NULL, phone);
                        makeEntry(&newPatient);
                    }
                }
                break;
                case 4:
                listAllPatients();
                break;
                case 5: {
                    Patient newPatient = makePatient(NULL, NULL);
                    makeEntry(&newPatient);
                    break;
                }
                case 6: {
                    char id[5];
                    printf("\nEnter ID of patient to edit: ");
                    scanf("%s", id);
                    editPatient(id);
                    break;
                }
                case 7: {
                    char id[5];
                    printf("\nEnter ID of patient to delete: ");
                    scanf("%s", id);
                    deletePatient(id);
                    break;
                }
                case 8:
                    return;
                default:
                printf("Invalid choice. Please try again.\n");
            }
        }
    }
}