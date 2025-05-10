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
// Helper function to check if a string is effectively empty (only whitespace)
static int isEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

void stripNewline(char* str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[--len] = '\0';
    }
}

static void getInput(const char* prompt, char* dest, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)size, stdin)) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        dest[0] = '\0';
    }
}

// Helper: set "N/A" if input is empty (for optional fields)
static void setOrNA(char* dest, const char* input, const size_t size) {
    if (!isEffectivelyEmpty(input))
        strncpy(dest, input, size - 1);
    else
        strncpy(dest, "N/A", size - 1);
    dest[size - 1] = '\0';
}
// Robust CSV parser: fills missing fields with "N/A"
void parseCsvLine(const char* line, Patient* patient) {
    char lineCopy[512];
    strncpy(lineCopy, line, sizeof(lineCopy) - 1);
    lineCopy[sizeof(lineCopy) - 1] = '\0';
    char* fields[11] = {0};
    int field = 0;
    char* p = lineCopy;
    char* start = p;
    while (*p && field < 11) {
        if (*p == ',') {
            *p = '\0';
            fields[field++] = start;
            start = p + 1;
        }
        p++;
    }
    if (field < 11) fields[field++] = start;
    for (int i = 0; i < 11; i++) {
        if (!fields[i] || isEffectivelyEmpty(fields[i])) fields[i] = "N/A";
        else stripNewline(fields[i]);
    }

    stripNewline(patient->name);
    stripNewline(patient->phone);
    stripNewline(patient->address);
    stripNewline(patient->email);
    stripNewline(patient->bloodType);
    stripNewline(patient->allergies);
    stripNewline(patient->emergencyContact);
    stripNewline(patient->primaryDoctor);

    patient->patientId = atoi(fields[0]);
    strncpy(patient->name, fields[1], sizeof(patient->name)-1);
    patient->age = atoi(fields[2]);
    patient->gender = fields[3][0];
    strncpy(patient->phone, fields[4], sizeof(patient->phone)-1);
    strncpy(patient->address, fields[5], sizeof(patient->address)-1);
    strncpy(patient->email, fields[6], sizeof(patient->email)-1);
    strncpy(patient->bloodType, fields[7], sizeof(patient->bloodType)-1);
    strncpy(patient->allergies, fields[8], sizeof(patient->allergies)-1);
    strncpy(patient->emergencyContact, fields[9], sizeof(patient->emergencyContact)-1);
    strncpy(patient->primaryDoctor, fields[10], sizeof(patient->primaryDoctor)-1);
}

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

        // Extract just the fields needed for searching
        const char* tok_id = strtok(lineCopy, ",");
        const char* tok_name = strtok(NULL, ",");
        strtok(NULL, ","); // age
        strtok(NULL, ","); // gender
        const char* tok_phone = strtok(NULL, ",");

        // Skip validation for search fields
        if (!tok_id || !tok_name || !tok_phone) continue;

        int id = atoi(tok_id);

        // Keep the original search logic
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
            // Use the robust parser for the complete line
            parseCsvLine(line, &patient);
            break; // Exit the loop once found
        }
    }

    fclose(fp);
    return patient;
}



void show(Patient* patient) {
    printf("\n==== Patient Information ====\n");
    printf("ID: %d\n", patient->patientId);
    printf("Name: %s\n", patient->name);
    printf("Age: %d\n", patient->age);
    printf("Gender: %s\n", patient->gender == 'M' || patient->gender == 'm' ? "Male" : "Female");
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

        const char *tok_id = strtok(lineCopy, ",");
        const char *tok_name = strtok(NULL, ",");
        const char *tok_age = strtok(NULL, ",");
        const char *tok_gender = strtok(NULL, ","); // gender
        const char *tok_phn = strtok(NULL, ",");
        strtok(NULL, ","); // address
        strtok(NULL, ","); // email
        strtok(NULL, ","); // blood
        strtok(NULL, ","); // allergies
        strtok(NULL, ","); // emergency
        const char *tok_doctor = strtok(NULL, "\r\n");

        if (!tok_id || !tok_name) continue;

        printf("%-8s %-12s %-5s %-10s %-15s %-12s\n",
               tok_id, tok_name, tok_age, tok_gender[0] == 'M' || tok_gender[0] == 'm' ? "Male" : "Female" , tok_phn,
               tok_doctor ? tok_doctor : "N/A");
        count++;
    }

    printf("\nTotal patients: %d\n", count);
    printf("Press Enter to return to menu...");
    getchar();

    fclose(fp);
}

Patient makePatient(const char* name, const char* phone) {
    Patient newPatient = {0};
    char buffer[256];

    printf("\n==== Add New Patient ====\n");
    // Name (required)
    if (name) {
        printf("Name: %s\n", name);
        strncpy(newPatient.name, name, sizeof(newPatient.name) - 1);
    } else {
        getInput("Name: ", buffer, sizeof(buffer));
        setOrNA(newPatient.name, buffer, sizeof(newPatient.name));
    }
    // Age (required)
    getInput("Age: ", buffer, sizeof(buffer));
    newPatient.age = atoi(buffer);
    // Gender (required)
    getInput("Gender (M/F): ", buffer, sizeof(buffer));
    newPatient.gender = toupper(buffer[0]);
    // Phone (required)
    if (phone) {
        printf("Phone Number: %s\n", phone);
        strncpy(newPatient.phone, phone, sizeof(newPatient.phone) - 1);
    } else {
        getInput("Phone Number: ", buffer, sizeof(buffer));
        setOrNA(newPatient.phone, buffer, sizeof(newPatient.phone));
    }
    // Validate required fields
    if (isEffectivelyEmpty(newPatient.name) || newPatient.gender == 0 || isEffectivelyEmpty(newPatient.phone) || !newPatient.age) {
        printf("These fields cannot be empty. Please try again.\n");
        printf("Press Enter to return to menu...");
        getchar();
        newPatient.patientId = -1;
        return newPatient;
    }
    // Optional fields
    getInput("Address: ", buffer, sizeof(buffer));
    setOrNA(newPatient.address, buffer, sizeof(newPatient.address));
    getInput("Email: ", buffer, sizeof(buffer));
    setOrNA(newPatient.email, buffer, sizeof(newPatient.email));
    getInput("Blood Type: ", buffer, sizeof(buffer));
    setOrNA(newPatient.bloodType, buffer, sizeof(newPatient.bloodType));
    getInput("Allergies: ", buffer, sizeof(buffer));
    setOrNA(newPatient.allergies, buffer, sizeof(newPatient.allergies));
    getInput("Emergency Contact: ", buffer, sizeof(buffer));
    setOrNA(newPatient.emergencyContact, buffer, sizeof(newPatient.emergencyContact));
    getInput("Primary Doctor: ", buffer, sizeof(buffer));
    setOrNA(newPatient.primaryDoctor, buffer, sizeof(newPatient.primaryDoctor));
    return newPatient;
}

void editPatient(const int patientID, Patient patient) {
    char id[5];
    char prompt[256], input[256];

    FILE *fp = fopen(DATAFILE, "r");
    if (!fp) { perror("Unable to open data file"); return; }
    FILE *temp = fopen("data/temp.csv", "w");
    if (!temp) { perror("Unable to create temporary file"); fclose(fp); return; }
    char line[512];
    int found = 0;

    printf("\n==== Editing Patient (ID: %d) ====\n", patient.patientId);
    printf("For each field, press Enter to keep current value or enter new data\n\n");


    snprintf(prompt, sizeof(prompt), "Name [%s]: ", patient.name);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.name, input, sizeof(patient.name));

    snprintf(prompt, sizeof(prompt), "Age [%d]: ", patient.age);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) patient.age = atoi(input);

    snprintf(prompt, sizeof(prompt), "Gender (M/F) [%c]: ", patient.gender);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) patient.gender = toupper(input[0]);

    snprintf(prompt, sizeof(prompt), "Phone [%s]: ", patient.phone);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.phone, input, sizeof(patient.phone));

    snprintf(prompt, sizeof(prompt), "Address [%s]: ", patient.address);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.address, input, sizeof(patient.address));

    snprintf(prompt, sizeof(prompt), "Email [%s]: ", patient.email);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.email, input, sizeof(patient.email));

    snprintf(prompt, sizeof(prompt), "Blood Type [%s]: ", patient.bloodType);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.bloodType, input, sizeof(patient.bloodType));

    snprintf(prompt, sizeof(prompt), "Allergies [%s]: ", patient.allergies);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.allergies, input, sizeof(patient.allergies));

    snprintf(prompt, sizeof(prompt), "Emergency Contact [%s]: ", patient.emergencyContact);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.emergencyContact, input, sizeof(patient.emergencyContact));

    snprintf(prompt, sizeof(prompt), "Primary Doctor [%s]: ", patient.primaryDoctor);
    getInput(prompt, input, sizeof(input));
    if (!isEffectivelyEmpty(input)) setOrNA(patient.primaryDoctor, input, sizeof(patient.primaryDoctor));

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);
        int currentId = atoi(strtok(lineCopy, ","));
        if (currentId == patientID) {
            found = 1;
            fprintf(temp, "%d,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s\n",
                    patient.patientId,
                    patient.name,
                    patient.age,
                    patient.gender,
                    patient.phone,
                    patient.address,
                    patient.email,
                    patient.bloodType,
                    patient.allergies,
                    patient.emergencyContact,
                    patient.primaryDoctor);
        } else {
            fputs(line, temp);
        }
    }
    fclose(fp);
    fclose(temp);
    if (found) {
        remove(DATAFILE);
        rename("data/temp.csv", DATAFILE);
        printf("Patient record updated successfully.\n");
    } else {
        remove("data/temp.csv");
        printf("Error updating patient.\n");
    }
    printf("Press Enter to return to menu...");
    getchar();
}


void deletePatient(const int id) {
    FILE *fp = fopen(DATAFILE, "r");
    if (!fp) {
        perror("Unable to open data file");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }
    FILE *temp = fopen("data/temp.csv", "w");
    if (!temp) {
        perror("Unable to create temporary file");
        fclose(fp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);
        int currentId = atoi(strtok(lineCopy, ","));
        if (currentId == id) {
            found = 1; // Skip writing this line (delete)
            continue;
        }
        fputs(line, temp);
    }
    fclose(fp);
    fclose(temp);

    if (found) {
        remove(DATAFILE);
        rename("data/temp.csv", DATAFILE);
        printf("Patient record deleted successfully.\n");
    } else {
        remove("data/temp.csv");
        printf("Patient with ID %d not found.\n", id);
    }
    printf("Press Enter to return to menu...");
    getchar();
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
                        if (newPatient.patientId == 0)
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
                        if (newPatient.patientId == 0)
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
                        if (newPatient.patientId == 0)
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
                    int id = 0;
                    printf("\nEnter ID of patient to edit: ");
                    scanf("%d", &id);
                    char pid[5];
                    sprintf(pid, "%d", id);
                    Patient exists = findPatient(3, pid);
                    if (!exists.patientId) {
                        printf("Patient with ID %s not found.\n", pid);
                        printf("Press Enter to return to menu...");
                        getchar();
                        break;
                    }
                    while (getchar() != '\n'){}
                    editPatient(id, exists);
                    break;
                }
                case 7: {
                    int id = 0;
                    printf("\nEnter ID of patient to delete: ");
                    scanf("%d", &id);
                    char pid[5];
                    sprintf(pid, "%d", id);
                    Patient exists = findPatient(3, pid);
                    if (!exists.patientId) {
                        printf("Patient with ID %s not found.\n", pid);
                        printf("Press Enter to return to menu...");
                        getchar();
                        break;
                    }
                    while (getchar() != '\n'){}
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