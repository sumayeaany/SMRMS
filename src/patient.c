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

Patient findPatient(const int searchType, const char* searchValue) {
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

        int found = 0;
        switch(searchType) {
            case 1: // Phone
                if (strcmp(patient.phone, searchValue) == 0) {
                    found = 1;
                }
                break;
            case 2: // Name (case-insensitive partial match)
                {
                    char nameLower[50], searchLower[50];
                    strcpy(nameLower, patient.name);
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
                if (patient.patientId == atoi(searchValue)) {
                    found = 1;
                }
                break;
            default:
                ;
        }

        if (found) {
            fclose(fp);
            return patient;
        }
    }
    fclose(fp);
    patient.patientId = 0; // Not found
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
    Patient patient = findPatient(3, patientIdStr);

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

void searchPatient() {
    char searchName[50];
    printf("Enter patient name to search: ");
    fgets(searchName, sizeof(searchName), stdin);
    stripPatientNewline(searchName);

    FILE *fp = fopen(PATIENT_DATAFILE, "r");
    if (!fp) {
        printf("No patient data file found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Patient patient;
    int found = 0;
    printf("\n==== Search Results ====\n");

    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &patient.patientId, patient.name, &patient.age, &patient.gender,
                  patient.phone, patient.address, patient.email, patient.bloodType,
                  patient.allergies, patient.emergencyContact, patient.primaryDoctor) >= 5) {
        if (strstr(patient.name, searchName) != NULL) {
            showPatient(&patient);
            found = 1;
        }
    }

    if (!found) {
        printf("No patients found matching '%s'.\n", searchName);
    }

    fclose(fp);
    printf("Press Enter to return to menu...");
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
        printf("6. Search by ID\n");
        printf("7. Search by Phone\n");
        printf("8. Back to Main Menu\n");
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
                scanf("%d", &patientId);
                getchar();

                char patientIdStr[10];
                sprintf(patientIdStr, "%d", patientId);
                Patient patient = findPatient(3, patientIdStr);

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
                scanf("%d", &patientId);
                getchar();
                deletePatient(patientId);
                break;
            }
            case 6: {
                char patientId[10];
                printf("Enter Patient ID: ");
                scanf("%s", patientId);
                getchar();
                Patient patient = findPatient(3, patientId);
                if (patient.patientId != 0) {
                    showPatient(&patient);
                    printf("Press Enter to return to menu...");
                    getchar();
                } else {
                    printf("Patient not found!\n");
                    printf("Press Enter to return to menu...");
                    getchar();
                }
                break;
            }
            case 7: {
                char phone[15];
                printf("Enter Phone Number: ");
                scanf("%s", phone);
                getchar();
                Patient patient = findPatient(1, phone);
                if (patient.patientId != 0) {
                    showPatient(&patient);
                    printf("Press Enter to return to menu...");
                    getchar();
                } else {
                    printf("Patient not found!\n");
                    printf("Press Enter to return to menu...");
                    getchar();
                }
                break;
            }
            case 8:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}