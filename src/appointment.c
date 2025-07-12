//
// Created by User on 07-Jul-25.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appointment.h"

#define APPOINTMENT_DATAFILE "data/appointment.csv"

// Static variable to store the maximum appointment ID
static int maxAppointmentId = 2000;
static int isMaxAppointmentIdInitialized = 0;

// Helper function to strip newlines
void stripAppointmentNewline(char* str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[--len] = '\0';
    }
}

// Helper function to check if a string is effectively empty
static int isAppointmentEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function for input
static void getAppointmentInput(const char* prompt, char* dest, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)size, stdin)) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        dest[0] = '\0';
    }
}

// Helper: set "N/A" if input is empty
static void setAppointmentOrNA(char* dest, const char* input, const size_t size) {
    if (!isAppointmentEffectivelyEmpty(input))
        strncpy(dest, input, size - 1);
    else
        strncpy(dest, "N/A", size - 1);
    dest[size - 1] = '\0';
}

void initializeMaxAppointmentId() {
    if (isMaxAppointmentIdInitialized) return;

    FILE *fp = fopen(APPOINTMENT_DATAFILE, "r");
    if (!fp) {
        maxAppointmentId = 2000; // Start from 2001 for the first appointment
        isMaxAppointmentIdInitialized = 1;
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        const int id = atoi(strtok(line, ","));
        if (id > maxAppointmentId) maxAppointmentId = id;
    }

    fclose(fp);
    isMaxAppointmentIdInitialized = 1;
}

int generateAppointmentId() {
    if (!isMaxAppointmentIdInitialized) {
        initializeMaxAppointmentId();
    }
    return ++maxAppointmentId;
}

Appointment makeAppointment(const int patientId, const char* doctorName, const char* date, const char* time, const char* purpose) {
    Appointment newAppointment = {0};
    char buffer[256];

    printf("\n==== Schedule New Appointment ====\n");

    // Patient ID
    if (patientId > 0) {
        printf("Patient ID: %d\n", patientId);
        newAppointment.patientId = patientId;
    } else {
        getAppointmentInput("Patient ID: ", buffer, sizeof(buffer));
        newAppointment.patientId = atoi(buffer);
    }

    // Doctor Name
    if (doctorName) {
        printf("Doctor Name: %s\n", doctorName);
        strncpy(newAppointment.doctorName, doctorName, sizeof(newAppointment.doctorName) - 1);
        newAppointment.doctorName[sizeof(newAppointment.doctorName) - 1] = '\0'; // Add this
    } else {
        getAppointmentInput("Doctor Name: ", buffer, sizeof(buffer));
        setAppointmentOrNA(newAppointment.doctorName, buffer, sizeof(newAppointment.doctorName));
    }

    // Date
    if (date) {
        printf("Date (DD/MM/YYYY): %s\n", date);
        strncpy(newAppointment.date, date, sizeof(newAppointment.date) - 1);
        newAppointment.date[sizeof(newAppointment.date) - 1] = '\0'; // Add this
    } else {
        getAppointmentInput("Date (DD/MM/YYYY): ", buffer, sizeof(buffer));
        setAppointmentOrNA(newAppointment.date, buffer, sizeof(newAppointment.date));
    }

    // Time
    if (time) {
        printf("Time (HH:MM): %s\n", time);
        strncpy(newAppointment.time, time, sizeof(newAppointment.time) - 1);
        newAppointment.time[sizeof(newAppointment.time) - 1] = '\0'; // Add this
    } else {
        getAppointmentInput("Time (HH:MM): ", buffer, sizeof(buffer));
        setAppointmentOrNA(newAppointment.time, buffer, sizeof(newAppointment.time));
    }

    // Purpose
    if (purpose) {
        printf("Purpose: %s\n", purpose);
        strncpy(newAppointment.purpose, purpose, sizeof(newAppointment.purpose) - 1);
        newAppointment.purpose[sizeof(newAppointment.purpose) - 1] = '\0'; // Add this
    } else {
        getAppointmentInput("Purpose: ", buffer, sizeof(buffer));
        setAppointmentOrNA(newAppointment.purpose, buffer, sizeof(newAppointment.purpose));
    }

    // Set default status
    strcpy(newAppointment.status, "Scheduled");

    return newAppointment;
}

Appointment findAppointment(const int appointmentId) {
    FILE *fp = fopen(APPOINTMENT_DATAFILE, "r");
    Appointment appointment = {0};

    if (!fp) {
        return appointment; // Return empty appointment if file doesn't exist
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        const int id = atoi(strtok(lineCopy, ","));
        if (id == appointmentId) {
            // Parse the full line
            char* fields[7] = {0};
            strcpy(lineCopy, line);

            char* p = lineCopy;
            char* start = p;
            int field = 0;

            while (*p && field < 7) {
                if (*p == ',') {
                    *p = '\0';
                    fields[field++] = start;
                    start = p + 1;
                }
                p++;
            }
            if (field < 7) fields[field++] = start;

            // Clean up fields
            for (int i = 0; i < 7; i++) {
                if (!fields[i] || isAppointmentEffectivelyEmpty(fields[i]))
                    fields[i] = "N/A";
                else
                    stripAppointmentNewline(fields[i]);
            }

            appointment.appointmentId = atoi(fields[0]);
            appointment.patientId = atoi(fields[1]);
            strncpy(appointment.doctorName, fields[2], sizeof(appointment.doctorName)-1);
            strncpy(appointment.date, fields[3], sizeof(appointment.date)-1);
            strncpy(appointment.time, fields[4], sizeof(appointment.time)-1);
            strncpy(appointment.purpose, fields[5], sizeof(appointment.purpose)-1);
            strncpy(appointment.status, fields[6], sizeof(appointment.status)-1);
            break;
        }
    }

    fclose(fp);
    return appointment;
}

void showAppointment(Appointment* appointment) {
    printf("\n==== Appointment Information ====\n");
    printf("Appointment ID: %d\n", appointment->appointmentId);
    printf("Patient ID: %d\n", appointment->patientId);
    printf("Doctor: %s\n", appointment->doctorName);
    printf("Date: %s\n", appointment->date);
    printf("Time: %s\n", appointment->time);
    printf("Purpose: %s\n", appointment->purpose);
    printf("Status: %s\n", appointment->status);

    printf("Press Enter to return to menu...");
    getchar();
}

void makeAppointmentEntry(Appointment* appointment) {
    FILE *fp = fopen(APPOINTMENT_DATAFILE, "a");
    if (!fp) {
        perror("Unable to open appointment data file");
        printf("DEBUG: Failed to open file: %s\n", APPOINTMENT_DATAFILE); // Add this
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    if (appointment->appointmentId == 0) {
        appointment->appointmentId = generateAppointmentId();
    }

    // Strip newlines before writing
    stripAppointmentNewline(appointment->doctorName);
    stripAppointmentNewline(appointment->date);
    stripAppointmentNewline(appointment->time);
    stripAppointmentNewline(appointment->purpose);
    stripAppointmentNewline(appointment->status);

    fprintf(fp, "%d,%d,%s,%s,%s,%s,%s\n",
            appointment->appointmentId,
            appointment->patientId,
            appointment->doctorName,
            appointment->date,
            appointment->time,
            appointment->purpose,
            appointment->status);

    fclose(fp);
    printf("Appointment scheduled successfully with ID: %d\n", appointment->appointmentId);
    printf("Press Enter to return to menu...");
    getchar();
}

void listAllAppointments() {
    FILE *fp = fopen(APPOINTMENT_DATAFILE, "r");
    if (!fp) {
        printf("No appointments found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    char line[512];
    int count = 0;

    printf("\n==== All Appointments ====\n");
    printf("%-8s %-10s %-15s %-12s %-8s %-15s %-12s\n",
           "App ID", "Patient ID", "Doctor", "Date", "Time", "Purpose", "Status");
    printf("--------------------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        const char *tok_id = strtok(lineCopy, ",");
        const char *tok_patientId = strtok(NULL, ",");
        const char *tok_doctor = strtok(NULL, ",");
        const char *tok_date = strtok(NULL, ",");
        const char *tok_time = strtok(NULL, ",");
        const char *tok_purpose = strtok(NULL, ",");
        const char *tok_status = strtok(NULL, "\r\n");

        if (!tok_id || !tok_patientId) continue;

        printf("%-8s %-10s %-15s %-12s %-8s %-15s %-12s\n",
               tok_id, tok_patientId,
               tok_doctor ? tok_doctor : "N/A",
               tok_date ? tok_date : "N/A",
               tok_time ? tok_time : "N/A",
               tok_purpose ? tok_purpose : "N/A",
               tok_status ? tok_status : "N/A");
        count++;
    }

    printf("\nTotal appointments: %d\n", count);
    printf("Press Enter to return to menu...");
    getchar();

    fclose(fp);
}

void markAppointmentAsComplete(const int appointmentId) {
    FILE *fp = fopen(APPOINTMENT_DATAFILE, "r");
    if (!fp) {
        perror("Unable to open appointment data file");
        return;
    }

    FILE *temp = fopen("data/temp_appointment.csv", "w");
    if (!temp) {
        perror("Unable to create temporary file");
        fclose(fp);
        return;
    }

    char line[512];
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        const char* id_str = strtok(lineCopy, ",");
        if (id_str == NULL) continue;

        const int currentId = atoi(id_str);

        if (currentId == appointmentId) {
            found = 1;
            // Reconstruct the line with the new status
            char* patientId_str = strtok(NULL, ",");
            char* doctorName_str = strtok(NULL, ",");
            char* date_str = strtok(NULL, ",");
            char* time_str = strtok(NULL, ",");
            char* purpose_str = strtok(NULL, ",");
            // The original status is ignored

            fprintf(temp, "%d,%s,%s,%s,%s,%s,%s\n",
                    currentId,
                    patientId_str ? patientId_str : "0",
                    doctorName_str ? doctorName_str : "N/A",
                    date_str ? date_str : "N/A",
                    time_str ? time_str : "N/A",
                    purpose_str ? purpose_str : "N/A",
                    "Completed");
        } else {
            fputs(line, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove(APPOINTMENT_DATAFILE);
        rename("data/temp_appointment.csv", APPOINTMENT_DATAFILE);
        printf("Appointment ID %d marked as complete.\n", appointmentId);
    } else {
        remove("data/temp_appointment.csv");
        printf("Appointment ID %d not found.\n", appointmentId);
    }
}

void editAppointment(const int appointmentId, Appointment* appointment) {
    char prompt[256], input[256];

    FILE *fp = fopen(APPOINTMENT_DATAFILE, "r");
    if (!fp) {
        perror("Unable to open appointment data file");
        return;
    }

    FILE *temp = fopen("data/temp_appointment.csv", "w");
    if (!temp) {
        perror("Unable to create temporary file");
        fclose(fp);
        return;
    }

    char line[512];
    int found = 0;

    printf("\n==== Editing Appointment (ID: %d) ====\n", appointment->appointmentId);
    printf("For each field, press Enter to keep current value or enter new data\n\n");

    snprintf(prompt, sizeof(prompt), "Patient ID [%d]: ", appointment->patientId);
    getAppointmentInput(prompt, input, sizeof(input));
    if (!isAppointmentEffectivelyEmpty(input)) appointment->patientId = atoi(input);

    snprintf(prompt, sizeof(prompt), "Doctor Name [%s]: ", appointment->doctorName);
    getAppointmentInput(prompt, input, sizeof(input));
    if (!isAppointmentEffectivelyEmpty(input)) setAppointmentOrNA(appointment->doctorName, input, sizeof(appointment->doctorName));

    snprintf(prompt, sizeof(prompt), "Date [%s]: ", appointment->date);
    getAppointmentInput(prompt, input, sizeof(input));
    if (!isAppointmentEffectivelyEmpty(input)) setAppointmentOrNA(appointment->date, input, sizeof(appointment->date));

    snprintf(prompt, sizeof(prompt), "Time [%s]: ", appointment->time);
    getAppointmentInput(prompt, input, sizeof(input));
    if (!isAppointmentEffectivelyEmpty(input)) setAppointmentOrNA(appointment->time, input, sizeof(appointment->time));

    snprintf(prompt, sizeof(prompt), "Purpose [%s]: ", appointment->purpose);
    getAppointmentInput(prompt, input, sizeof(input));
    if (!isAppointmentEffectivelyEmpty(input)) setAppointmentOrNA(appointment->purpose, input, sizeof(appointment->purpose));

    snprintf(prompt, sizeof(prompt), "Status [%s]: ", appointment->status);
    getAppointmentInput(prompt, input, sizeof(input));
    if (!isAppointmentEffectivelyEmpty(input)) setAppointmentOrNA(appointment->status, input, sizeof(appointment->status));

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);
        const int currentId = atoi(strtok(lineCopy, ","));

        if (currentId == appointmentId) {
            found = 1;
            fprintf(temp, "%d,%d,%s,%s,%s,%s,%s\n",
                    appointment->appointmentId,
                    appointment->patientId,
                    appointment->doctorName,
                    appointment->date,
                    appointment->time,
                    appointment->purpose,
                    appointment->status);
        } else {
            fputs(line, temp);
        }
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove(APPOINTMENT_DATAFILE);
        rename("data/temp_appointment.csv", APPOINTMENT_DATAFILE);
        printf("Appointment updated successfully.\n");
    } else {
        remove("data/temp_appointment.csv");
        printf("Error updating appointment.\n");
    }

    printf("Press Enter to return to menu...");
    getchar();
}

void deleteAppointment(const int appointmentId) {
    FILE *fp = fopen(APPOINTMENT_DATAFILE, "r");
    if (!fp) {
        perror("Unable to open appointment data file");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    FILE *temp = fopen("data/temp_appointment.csv", "w");
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
        const int currentId = atoi(strtok(lineCopy, ","));

        if (currentId == appointmentId) {
            found = 1; // Skip writing this line (delete)
            continue;
        }
        fputs(line, temp);
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove(APPOINTMENT_DATAFILE);
        rename("data/temp_appointment.csv", APPOINTMENT_DATAFILE);
        printf("Appointment deleted successfully.\n");
    } else {
        remove("data/temp_appointment.csv");
        printf("Appointment with ID %d not found.\n", appointmentId);
    }

    printf("Press Enter to return to menu...");
    getchar();
}

void appointmentInformationLookup() {
    int choice;

    while (1) {
        system("cls");

        printf("\n==== Doctor Appointment Management ====\n");
        printf("1. Schedule New Appointment\n");
        printf("2. Search Appointment by ID\n");
        printf("3. Complete Appointment\n");
        printf("4. Edit Appointment\n");
        printf("5. Cancel Appointment\n");
        printf("6. List all Appointments\n");
        printf("7. Retutn to Previous Menu\n");
        printf("\nChoice: ");
        fflush(stdout);

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n') {}
            continue;
        }
        while (getchar() != '\n') {}

        switch (choice) {
            case 1: {
                Appointment newAppointment = makeAppointment(0, NULL, NULL, NULL, NULL);
                makeAppointmentEntry(&newAppointment);
                break;
            }
            case 2: {
                int id;
                printf("\nEnter appointment ID: ");
                scanf("%d", &id);
                while (getchar() != '\n') {}

                Appointment appointment = findAppointment(id);
                if (appointment.appointmentId)
                    showAppointment(&appointment);
                else {
                    printf("Appointment not found!\n");
                    printf("Press Enter to return to menu...");
                    getchar();
                }
                break;
            }
            case 3:
                int id;
                printf("\nEnter appointment ID: ");
                scanf("%d", &id);
                while (getchar() != '\n') {}

                Appointment appointment = findAppointment(id);
                if (appointment.appointmentId)
                    markAppointmentAsComplete(id);
                else {
                    printf("Appointment not found!\n");
                    printf("Press Enter to return to menu...");
                    getchar();
                }
                break;
            case 4: {
                int id;
                printf("\nEnter ID of appointment to edit: ");
                scanf("%d", &id);
                while (getchar() != '\n') {}

                Appointment exists = findAppointment(id);
                if (!exists.appointmentId) {
                    printf("Appointment with ID %d not found.\n", id);
                    printf("Press Enter to return to menu...");
                    getchar();
                    break;
                }
                editAppointment(id, &exists);
                break;
            }
            case 5: {
                int id;
                printf("\nEnter ID of appointment to cancel: ");
                scanf("%d", &id);
                while (getchar() != '\n') {}

                const Appointment exists = findAppointment(id);
                if (!exists.appointmentId) {
                    printf("Appointment with ID %d not found.\n", id);
                    printf("Press Enter to return to menu...");
                    getchar();
                    break;
                }
                deleteAppointment(id);
                break;
            }
            case 6:
                listAllAppointments();
                break;
            case 7:
                return;
            default:
                printf("Invalid choice. Please try again.\n");
                printf("Press Enter to continue...");
                getchar();
        }
    }
}