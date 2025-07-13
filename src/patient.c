#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "patient.h"

#define PATIENT_DATAFILE "data/patient.csv"

static int maxPatientId = 1000;
static int isMaxPatientIdInitialized = 0;

// Helper function to check if a string is effectively empty
static int isPatientEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function for input
static void getPatientInput(const char* prompt, char* dest, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)size, stdin)) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        dest[0] = '\0';
    }
}

// Helper: set "N/A" if input is empty
static void setPatientOrNA(char* dest, const char* input, const size_t size) {
    if (!isPatientEffectivelyEmpty(input))
        strncpy(dest, input, size - 1);
    else
        strncpy(dest, "N/A", size - 1);
    dest[size - 1] = '\0';
}
Patient findPatientByNameAndPhone(const char* name, const char* phone) {
    Patient patient = {0};
    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    if (!fp) {
        printf("Unable to open patient data file\n");
        return patient;
    }

    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &patient.patientId, patient.name, &patient.age, &patient.gender,
                  patient.phone, patient.address, patient.email, patient.bloodType,
                  patient.allergies, patient.emergencyContact, patient.primaryDoctor) >= 5) {

        // Case-insensitive name comparison
        char nameLower[50], searchLower[50];
        strcpy(nameLower, patient.name);
        strcpy(searchLower, name);

        for (int i = 0; nameLower[i]; i++)
            nameLower[i] = tolower((int)nameLower[i]);
        for (int i = 0; searchLower[i]; i++)
            searchLower[i] = tolower((int)searchLower[i]);

        // Check if both name and phone match
        if (strstr(nameLower, searchLower) && strcmp(patient.phone, phone) == 0) {
            fclose(fp);
            return patient;
        }
    }

    fclose(fp);
    patient.patientId = 0; // Not found
    return patient;
}

int searchAndShowPatientsByName(const char* name) {
    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    if (!fp) {
        printf("Unable to open patient data file\n");
        return 0;
    }

    Patient patient;
    int found = 0;
    printf("\n==== Patients Found by Name ====\n");
    printf("%-5s %-20s %-5s %-8s %-15s %-20s\n", "ID", "Name", "Age", "Gender", "Phone", "Primary Doctor");
    printf("------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &patient.patientId, patient.name, &patient.age, &patient.gender,
                  patient.phone, patient.address, patient.email, patient.bloodType,
                  patient.allergies, patient.emergencyContact, patient.primaryDoctor) >= 5) {

        // Case-insensitive name comparison
        char nameLower[50], searchLower[50];
        strcpy(nameLower, patient.name);
        strcpy(searchLower, name);

        for (int i = 0; nameLower[i]; i++)
            nameLower[i] = tolower((int)nameLower[i]);
        for (int i = 0; searchLower[i]; i++)
            searchLower[i] = tolower((int)searchLower[i]);

        if (strstr(nameLower, searchLower)) {
            printf("%-5d %-20s %-5d %-8s %-15s %-20s\n",
                   patient.patientId, patient.name, patient.age,
                   patient.gender == 'M' ? "Male" : "Female", patient.phone, patient.primaryDoctor);
            found = 1;
        }
    }

    fclose(fp);

    if (found) {
        printf("\nMultiple patients found. Please use ID or provide phone number for exact match.\n");
    }

    return found;
}
void initializeMaxPatientId() {
    if (isMaxPatientIdInitialized) return;

    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    if (!fp) {
        maxPatientId = 1000;
        isMaxPatientIdInitialized = 1;
        return;
    }

    Patient patient;
    maxPatientId = 1000;
    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &patient.patientId, patient.name, &patient.age, &patient.gender,
                  patient.phone, patient.address, patient.email, patient.bloodType,
                  patient.allergies, patient.emergencyContact, patient.primaryDoctor) >= 5) {
        if (patient.patientId > maxPatientId) {
            maxPatientId = patient.patientId;
        }
    }
    fclose(fp);
    isMaxPatientIdInitialized = 1;
}

int generatePatientId() {
    if (!isMaxPatientIdInitialized) {
        initializeMaxPatientId();
    }
    return ++maxPatientId;
}

void stripPatientNewline(char* str) {
    const size_t len = strlen(str);
    if (len > 0 && str[len-1] == '\n') {
        str[len-1] = '\0';
    }
}

void makePatientEntry(Patient* patient) {
    // Create data directory if it doesn't exist
    #ifdef _WIN32
        system("if not exist data mkdir data");
    #else
        system("mkdir -p data");
    #endif

    FILE *fp = fopen(PATIENT_DATAFILE, "a");
    if (!fp) {
        fp = fopen(PATIENT_DATAFILE, "w");
        if (!fp) {
            perror("Unable to create patient data file");
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }
        fclose(fp);
        fp = fopen(PATIENT_DATAFILE, "a");
    }

    if (patient->patientId == 0) {
        patient->patientId = generatePatientId();
    }

    fprintf(fp, "%d,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s\n",
            patient->patientId, patient->name, patient->age, patient->gender,
            patient->phone, patient->address, patient->email, patient->bloodType,
            patient->allergies, patient->emergencyContact, patient->primaryDoctor);

    fclose(fp);
    printf("Patient added successfully with ID: %d\n", patient->patientId);
    printf("Press Enter to return to menu...");
    getchar();
}

Patient makePatient() {
    Patient patient = {0};
    char buffer[256];

    printf("\n==== Add New Patient ====\n");

    // Patient name
    getPatientInput("Patient name: ", buffer, sizeof(buffer));
    if (isPatientEffectivelyEmpty(buffer)) {
        printf("Name cannot be empty. Please try again.\n");
        printf("Press Enter to return to menu...");
        getchar();
        patient.patientId = -1;
        return patient;
    }
    setPatientOrNA(patient.name, buffer, sizeof(patient.name));

    // Age
    getPatientInput("Age: ", buffer, sizeof(buffer));
    patient.age = atoi(buffer);
    if (patient.age <= 0) {
        printf("Invalid age. Please try again.\n");
        printf("Press Enter to return to menu...");
        getchar();
        patient.patientId = -1;
        return patient;
    }

    // Gender
    getPatientInput("Gender (M/F): ", buffer, sizeof(buffer));
    patient.gender = toupper(buffer[0]);
    if (patient.gender != 'M' && patient.gender != 'F') {
        printf("Invalid gender. Please try again.\n");
        printf("Press Enter to return to menu...");
        getchar();
        patient.patientId = -1;
        return patient;
    }

    // Phone
    getPatientInput("Phone number: ", buffer, sizeof(buffer));
    if (isPatientEffectivelyEmpty(buffer)) {
        printf("Phone number cannot be empty. Please try again.\n");
        printf("Press Enter to return to menu...");
        getchar();
        patient.patientId = -1;
        return patient;
    }
    setPatientOrNA(patient.phone, buffer, sizeof(patient.phone));

    // Optional fields
    getPatientInput("Address: ", buffer, sizeof(buffer));
    setPatientOrNA(patient.address, buffer, sizeof(patient.address));

    getPatientInput("Email: ", buffer, sizeof(buffer));
    setPatientOrNA(patient.email, buffer, sizeof(patient.email));

    getPatientInput("Blood type: ", buffer, sizeof(buffer));
    setPatientOrNA(patient.bloodType, buffer, sizeof(patient.bloodType));

    getPatientInput("Allergies: ", buffer, sizeof(buffer));
    setPatientOrNA(patient.allergies, buffer, sizeof(patient.allergies));

    getPatientInput("Emergency contact: ", buffer, sizeof(buffer));
    setPatientOrNA(patient.emergencyContact, buffer, sizeof(patient.emergencyContact));

    getPatientInput("Primary doctor: ", buffer, sizeof(buffer));
    setPatientOrNA(patient.primaryDoctor, buffer, sizeof(patient.primaryDoctor));

    return patient;
}


void showPatient(Patient* patient) {
    printf("\n==== Patient Details ====\n");
    printf("Patient ID: %d\n", patient->patientId);
    printf("Name: %s\n", patient->name);
    printf("Age: %d\n", patient->age);
    printf("Gender: %s\n", patient->gender == 'M' ? "Male" : "Female");
    printf("Phone: %s\n", patient->phone);
    printf("Address: %s\n", patient->address);
    printf("Email: %s\n", patient->email);
    printf("Blood Type: %s\n", patient->bloodType);
    printf("Allergies: %s\n", patient->allergies);
    printf("Emergency Contact: %s\n", patient->emergencyContact);
    printf("Primary Doctor: %s\n", patient->primaryDoctor);
    printf("==========================\n");
}

void listAllPatients() {
    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    if (!fp) {
        printf("No patient data file found or unable to open file.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Patient patient;
    printf("\n==== All Patients ====\n");
    printf("%-5s %-20s %-5s %-8s %-15s %-20s\n", "ID", "Name", "Age", "Gender", "Phone", "Primary Doctor");
    printf("------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &patient.patientId, patient.name, &patient.age, &patient.gender,
                  patient.phone, patient.address, patient.email, patient.bloodType,
                  patient.allergies, patient.emergencyContact, patient.primaryDoctor) >= 5) {
        printf("%-5d %-20s %-5d %-8s %-15s %-20s\n",
               patient.patientId, patient.name, patient.age,
               patient.gender == 'M' ? "Male" : "Female", patient.phone, patient.primaryDoctor);
    }
    fclose(fp);
    printf("\nPress Enter to return to menu...");
    getchar();
}

void editPatient(const int patientId, Patient* patient) {
    char prompt[256], input[256];

    printf("\n==== Editing Patient (ID: %d) ====\n", patient->patientId);
    printf("For each field, press Enter to keep current value or enter new data\n\n");

    snprintf(prompt, sizeof(prompt), "Name [%s]: ", patient->name);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->name, input, sizeof(patient->name));

    snprintf(prompt, sizeof(prompt), "Age [%d]: ", patient->age);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) patient->age = atoi(input);

    snprintf(prompt, sizeof(prompt), "Gender (M/F) [%c]: ", patient->gender);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) patient->gender = toupper(input[0]);

    snprintf(prompt, sizeof(prompt), "Phone [%s]: ", patient->phone);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->phone, input, sizeof(patient->phone));

    snprintf(prompt, sizeof(prompt), "Address [%s]: ", patient->address);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->address, input, sizeof(patient->address));

    snprintf(prompt, sizeof(prompt), "Email [%s]: ", patient->email);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->email, input, sizeof(patient->email));

    snprintf(prompt, sizeof(prompt), "Blood Type [%s]: ", patient->bloodType);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->bloodType, input, sizeof(patient->bloodType));

    snprintf(prompt, sizeof(prompt), "Allergies [%s]: ", patient->allergies);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->allergies, input, sizeof(patient->allergies));

    snprintf(prompt, sizeof(prompt), "Emergency Contact [%s]: ", patient->emergencyContact);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->emergencyContact, input, sizeof(patient->emergencyContact));

    snprintf(prompt, sizeof(prompt), "Primary Doctor [%s]: ", patient->primaryDoctor);
    getPatientInput(prompt, input, sizeof(input));
    if (!isPatientEffectivelyEmpty(input)) setPatientOrNA(patient->primaryDoctor, input, sizeof(patient->primaryDoctor));

    // Update the file
    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    FILE *temp = fopen("data/temp_patient.csv", "w");

    if (!fp || !temp) {
        printf("Error accessing files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Patient currentPatient;
    int found = 0;
    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &currentPatient.patientId, currentPatient.name, &currentPatient.age, &currentPatient.gender,
                  currentPatient.phone, currentPatient.address, currentPatient.email, currentPatient.bloodType,
                  currentPatient.allergies, currentPatient.emergencyContact, currentPatient.primaryDoctor) >= 5) {

        if (currentPatient.patientId == patientId) {
            found = 1;
            // Write updated record
            fprintf(temp, "%d,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s\n",
                    patient->patientId, patient->name, patient->age, patient->gender,
                    patient->phone, patient->address, patient->email, patient->bloodType,
                    patient->allergies, patient->emergencyContact, patient->primaryDoctor);
        } else {
            // Write original record
            fprintf(temp, "%d,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s\n",
                    currentPatient.patientId, currentPatient.name, currentPatient.age, currentPatient.gender,
                    currentPatient.phone, currentPatient.address, currentPatient.email, currentPatient.bloodType,
                    currentPatient.allergies, currentPatient.emergencyContact, currentPatient.primaryDoctor);
        }
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove(PATIENT_DATAFILE);
        rename("data/temp_patient.csv", PATIENT_DATAFILE);
        printf("Patient updated successfully.\n");
    } else {
        remove("data/temp_patient.csv");
        printf("Error updating patient.\n");
    }

    printf("Press Enter to return to menu...");
    getchar();
}

void deletePatient(const int patientId) {
    char patientIdStr[10];
    sprintf(patientIdStr, "%d", patientId);
    Patient patient = findPatientBySearch(1, patientIdStr, NULL);

    if (patient.patientId == 0) {
        printf("Patient with ID %d not found.\n", patientId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    showPatient(&patient);
    printf("\nAre you sure you want to delete this patient? (y/n): ");
    char confirm;
    scanf("%c", &confirm);
    getchar();

    if (confirm != 'y' && confirm != 'Y') {
        printf("Delete operation cancelled.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    FILE *temp = fopen("data/temp_patient.csv", "w");

    if (!fp || !temp) {
        printf("Error accessing files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Patient currentPatient;
    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &currentPatient.patientId, currentPatient.name, &currentPatient.age, &currentPatient.gender,
                  currentPatient.phone, currentPatient.address, currentPatient.email, currentPatient.bloodType,
                  currentPatient.allergies, currentPatient.emergencyContact, currentPatient.primaryDoctor) >= 5) {

        // Only write records that don't match the ID to delete
        if (currentPatient.patientId != patientId) {
            fprintf(temp, "%d,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s\n",
                    currentPatient.patientId, currentPatient.name, currentPatient.age, currentPatient.gender,
                    currentPatient.phone, currentPatient.address, currentPatient.email, currentPatient.bloodType,
                    currentPatient.allergies, currentPatient.emergencyContact, currentPatient.primaryDoctor);
        }
    }

    fclose(fp);
    fclose(temp);

    // Replace original file with updated file
    remove(PATIENT_DATAFILE);
    rename("data/temp_patient.csv", PATIENT_DATAFILE);

    printf("Patient deleted successfully.\n");
    printf("Press Enter to return to menu...");
    getchar();
}

Patient findPatientBySearch(int searchType, const char* value1, const char* value2) {
    Patient patient = {0};
    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    if (!fp) {
        return patient;
    }

    Patient currentPatient;
    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &currentPatient.patientId, currentPatient.name, &currentPatient.age, &currentPatient.gender,
                  currentPatient.phone, currentPatient.address, currentPatient.email, currentPatient.bloodType,
                  currentPatient.allergies, currentPatient.emergencyContact, currentPatient.primaryDoctor) >= 5) {

        switch (searchType) {
            case 1: // Search by ID
                if (currentPatient.patientId == atoi(value1)) {
                    fclose(fp);
                    return currentPatient;
                }
                break;
            case 2: // Search by phone only
                if (strcmp(currentPatient.phone, value1) == 0) {
                    fclose(fp);
                    return currentPatient;
                }
                break;
            case 3: // Search by name AND phone (both must match)
                if (value2 != NULL) {
                    // Case-insensitive name comparison
                    char nameLower[50], searchLower[50];
                    strcpy(nameLower, currentPatient.name);
                    strcpy(searchLower, value1);

                    for (int i = 0; nameLower[i]; i++)
                        nameLower[i] = tolower((int)nameLower[i]);
                    for (int i = 0; searchLower[i]; i++)
                        searchLower[i] = tolower((int)searchLower[i]);

                    if (strstr(nameLower, searchLower) && strcmp(currentPatient.phone, value2) == 0) {
                        fclose(fp);
                        return currentPatient;
                    }
                    break;
                }
            default:
                ;
        }
    }

    fclose(fp);
    return patient; // Return empty patient if not found
}

void searchPatient() {
    char searchValue[100];
    char phoneValue[20];
    Patient patient = {0};
    int found = 0;

    printf("\n==== Search Patient ====\n");
    printf("Enter Patient ID (or press Enter to search by name/phone): ");
    fgets(searchValue, sizeof(searchValue), stdin);
    searchValue[strcspn(searchValue, "\n")] = 0;

    // First try to search by ID if a number is provided
    if (!isPatientEffectivelyEmpty(searchValue) && atoi(searchValue) > 0) {
        patient = findPatientBySearch(1, searchValue, NULL); // Search by ID
        if (patient.patientId != 0) {
            printf("\n==== Patient Found by ID ====\n");
            showPatient(&patient);
            found = 1;
        }
    }

    // If not found by ID or no ID provided, search by name AND phone
    if (!found) {
        printf("Enter Patient Name: ");
        fgets(searchValue, sizeof(searchValue), stdin);
        searchValue[strcspn(searchValue, "\n")] = 0;

        if (!isPatientEffectivelyEmpty(searchValue)) {
            printf("Enter Phone Number: ");
            fgets(phoneValue, sizeof(phoneValue), stdin);
            phoneValue[strcspn(phoneValue, "\n")] = 0;

            if (!isPatientEffectivelyEmpty(phoneValue)) {
                // Search by both name and phone (both must match)
                patient = findPatientBySearch(3, searchValue, phoneValue);
                if (patient.patientId != 0) {
                    printf("\n==== Patient Found by Name and Phone ====\n");
                    showPatient(&patient);
                    found = 1;
                }
            } else {
                // Search by name only and show all matches
                found = searchAndShowPatientsByName(searchValue);
            }
        }
    }

    // If still not found, search by phone only
    if (!found) {
        printf("Enter Phone Number: ");
        fgets(searchValue, sizeof(searchValue), stdin);
        searchValue[strcspn(searchValue, "\n")] = 0;

        if (!isPatientEffectivelyEmpty(searchValue)) {
            patient = findPatientBySearch(2, searchValue, NULL); // Search by phone
            if (patient.patientId != 0) {
                printf("\n==== Patient Found by Phone ====\n");
                showPatient(&patient);
                found = 1;
            }
        }
    }

    if (!found) {
        printf("No patient found with the provided information.\n");
    }

    printf("\nPress Enter to return to menu...");
    getchar();
}

void patientInformationLookup() {
    initializeMaxPatientId();
    int choice;

    while (1) {
        system("cls");
        printf("==== Patient Information ====\n\n");
        printf("1. Add Patient\n");
        printf("2. Search Patient\n");
        printf("3. List All Patients\n");
        printf("4. Edit Patient\n");
        printf("5. Delete Patient\n");
        printf("6. Back to Main Menu\n");
        printf("\nChoice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n') {}
            printf("Invalid input. Press Enter to continue...");
            getchar();
            continue;
        }
        getchar(); // consume newline

        switch (choice) {
            case 1: {
                Patient patient = makePatient();
                if (patient.patientId != -1) {
                    makePatientEntry(&patient);
                }
                break;
            }
            case 2:
                searchPatient();
                break;
            case 3:
                listAllPatients();
                break;
            case 4: {
                int patientId;
                printf("Enter Patient ID to edit: ");
                if (scanf("%d", &patientId) != 1) {
                    while (getchar() != '\n'); // Clear buffer
                    printf("Invalid input.\n");
                    printf("Press Enter to continue...");
                    getchar();
                    break;
                }
                getchar();

                char patientIdStr[10];
                sprintf(patientIdStr, "%d", patientId);
                Patient patient = findPatientBySearch(1, patientIdStr, NULL); // Search by ID

                if (patient.patientId == 0) {
                    printf("Patient with ID %d not found.\n", patientId);
                    printf("Press Enter to return to menu...");
                    getchar();
                    break;
                }
                editPatient(patientId, &patient);
                break;
            }
            case 5: {
                int patientId;
                printf("Enter Patient ID to delete: ");
                if (scanf("%d", &patientId) != 1) {
                    while (getchar() != '\n'); // Clear buffer
                    printf("Invalid input.\n");
                    printf("Press Enter to continue...");
                    getchar();
                    break;
                }
                getchar();
                deletePatient(patientId);
                break;
            }
            case 6:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}