#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "emergency.h"
#include "patient.h"
#include "appointment.h"
#include "medicine.h"

#define EMERGENCY_DATAFILE "data/emergency.csv"
#define EMERGENCY_MEDICINE_DATAFILE "data/emergency_medicines.csv"

static EmergencyQueue emergencyQueue;
static int maxEmergencyId = 5000;
static int isQueueInitialized = 0;

static int tempPatientIdCounter = -1;

int generateTempPatientId() {
    return tempPatientIdCounter--;
}
// Helper function to check if a string is effectively empty
static int isEmergencyEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function for input
static void getEmergencyInput(const char* prompt, char* dest, const size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)size, stdin)) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        dest[0] = '\0';
    }
}

// Helper: set "N/A" if input is empty
static void setEmergencyOrNA(char* dest, const char* input, const size_t size) {
    if (!isEmergencyEffectivelyEmpty(input))
        strncpy(dest, input, size - 1);
    else
        strncpy(dest, "N/A", size - 1);
    dest[size - 1] = '\0';
}

void getCurrentDateTime(char* date, char* ttime) {
    time_t now;
    time(&now);
    const struct tm* tm_info = localtime(&now);

    strftime(date, 11, "%d/%m/%Y", tm_info);
    strftime(ttime, 9, "%H:%M:%S", tm_info);
}

int generateEmergencyId() {
    return ++maxEmergencyId;
}

const char* getPriorityString(const EmergencyPriority priority) {
    switch(priority) {
        case CRITICAL: return "CRITICAL";
        case HIGH: return "HIGH";
        case MEDIUM: return "MEDIUM";
        case LOW: return "LOW";
        default: return "UNKNOWN";
    }
}

void initializeEmergencyQueue() {
    if (isQueueInitialized) return;

    emergencyQueue.front = 0;
    emergencyQueue.rear = -1;
    emergencyQueue.count = 0;
    isQueueInitialized = 1;
}

int isEmergencyQueueEmpty() {
    return emergencyQueue.count == 0;
}

int isEmergencyQueueFull() {
    return emergencyQueue.count == MAX_EMERGENCY_QUEUE;
}

void enqueueEmergencyPatient(const EmergencyPatient patient) {
    if (isEmergencyQueueFull()) {
        printf("Emergency queue is full! Cannot add more patients.\n");
        return;
    }

    emergencyQueue.rear = (emergencyQueue.rear + 1) % MAX_EMERGENCY_QUEUE;
    emergencyQueue.patients[emergencyQueue.rear] = patient;
    emergencyQueue.count++;

    // Sort queue by priority after adding
    sortQueueByPriority();
}

EmergencyPatient dequeueEmergencyPatient() {
    EmergencyPatient patient = {0};

    if (isEmergencyQueueEmpty()) {
        printf("Emergency queue is empty!\n");
        return patient;
    }

    patient = emergencyQueue.patients[emergencyQueue.front];
    emergencyQueue.front = (emergencyQueue.front + 1) % MAX_EMERGENCY_QUEUE;
    emergencyQueue.count--;

    return patient;
}

void sortQueueByPriority() {
    if (emergencyQueue.count <= 1) return;

    EmergencyPatient temp[MAX_EMERGENCY_QUEUE];
    int tempCount = 0;

    // Copy all patients to temporary array
    for (int i = 0; i < emergencyQueue.count; i++) {
        const int index = (emergencyQueue.front + i) % MAX_EMERGENCY_QUEUE;
        temp[tempCount++] = emergencyQueue.patients[index];
    }

    // Sort by priority (lower number = higher priority)
    for (int i = 0; i < tempCount - 1; i++) {
        for (int j = 0; j < tempCount - i - 1; j++) {
            if (temp[j].priority > temp[j + 1].priority) {
                const EmergencyPatient swap = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = swap;
            }
        }
    }

    // Copy back to queue
    emergencyQueue.front = 0;
    emergencyQueue.rear = tempCount - 1;
    for (int i = 0; i < tempCount; i++) {
        emergencyQueue.patients[i] = temp[i];
    }
}

EmergencyPatient createEmergencyEntry() {
    EmergencyPatient patient = {0};
    char buffer[256];

    printf("\n==== Emergency Patient Registration ====\n");

    // Get patient ID or register new patient
    getEmergencyInput("Enter Patient ID (0 for new patient): ", buffer, sizeof(buffer));
    int patientId = atoi(buffer);

    if (patientId > 0) {
        // Find existing patient
        char idStr[10];
        sprintf(idStr, "%d", patientId);
        const Patient existingPatient = findPatientBySearch(3, idStr, NULL);

        if (existingPatient.patientId != 0) {
            patient.patientId = existingPatient.patientId;
            strncpy(patient.patientName, existingPatient.name, sizeof(patient.patientName) - 1);
            strncpy(patient.patientPhone, existingPatient.phone, sizeof(patient.patientPhone) - 1);
            printf("Found existing patient: %s\n", patient.patientName);
        } else {
            printf("Patient ID not found. Please register as new patient.\n");
            patientId = 0;
        }
    }

    if (patientId == 0) {
        // Register new patient with basic info
        getEmergencyInput("Patient Name: ", buffer, sizeof(buffer));
        if (isEmergencyEffectivelyEmpty(buffer)) {
            printf("Name cannot be empty!\n");
            patient.emergencyId = -1;
            return patient;
        }
        setEmergencyOrNA(patient.patientName, buffer, sizeof(patient.patientName));

        getEmergencyInput("Patient Phone: ", buffer, sizeof(buffer));
        if (isEmergencyEffectivelyEmpty(buffer)) {
            printf("Phone cannot be empty!\n");
            patient.emergencyId = -1;
            return patient;
        }
        setEmergencyOrNA(patient.patientPhone, buffer, sizeof(patient.patientPhone));

        patient.patientId = 0; // Will be assigned when full registration is done later
    }

    // Emergency-specific information
    getEmergencyInput("Symptoms/Chief Complaint: ", buffer, sizeof(buffer));
    setEmergencyOrNA(patient.symptoms, buffer, sizeof(patient.symptoms));

    // Priority assessment
    printf("\nPriority Assessment:\n");
    printf("1. CRITICAL (Life-threatening)\n");
    printf("2. HIGH (Urgent)\n");
    printf("3. MEDIUM (Semi-urgent)\n");
    printf("4. LOW (Non-urgent)\n");
    getEmergencyInput("Select Priority (1-4): ", buffer, sizeof(buffer));
    const int priority = atoi(buffer);
    patient.priority = (priority >= 1 && priority <= 4) ? (EmergencyPriority)priority : MEDIUM;

    // Set timestamps and status
    getCurrentDateTime(patient.arrivalDate, patient.arrivalTime);
    strcpy(patient.status, "Waiting");
    strcpy(patient.treatingDoctor, "N/A");
    strcpy(patient.treatment, "N/A");
    patient.medicineCount = 0;
    strcpy(patient.dischargeTime, "N/A");
    strcpy(patient.notes, "N/A");

    patient.emergencyId = generateEmergencyId();

    return patient;
}

void addPatientToEmergencyQueue() {
    if (isEmergencyQueueFull()) {
        printf("\nEmergency queue is full. Cannot add more patients.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    EmergencyPatient newPatient = {0};
    char buffer[256];
    int patientId = 0;
    Patient existingPatient = {0};

    printf("\n==== Add New Emergency Patient ====\n");
    printf("Is the patient already registered? (y/n): ");
    char choice;
    scanf(" %c", &choice);
    getchar();

    if (choice == 'y' || choice == 'Y') {
        printf("Enter Patient ID: ");
        if (scanf("%d", &patientId) != 1) {
            while (getchar() != '\n'); // Clear buffer
            printf("Invalid Patient ID.\n");
            return;
        }
        getchar();

        // Convert patientId to string for searching
        char patientIdStr[20];
        sprintf(patientIdStr, "%d", patientId);

        // Correctly call findPatientBySearch with a string
        existingPatient = findPatientBySearch(1, patientIdStr, NULL); // 1 for search by ID

        if (existingPatient.patientId == 0) {
            printf("Patient with ID %d not found in the system.\n", patientId);
            printf("Proceeding with manual entry.\n");
            patientId = 0; // Reset patientId to indicate not found
        } else {
            printf("Patient Found: %s\n", existingPatient.name);
            newPatient.patientId = existingPatient.patientId;
            strncpy(newPatient.patientName, existingPatient.name, sizeof(newPatient.patientName) - 1);
            strncpy(newPatient.patientPhone, existingPatient.phone, sizeof(newPatient.patientPhone) - 1);
        }
    }

    if (patientId == 0) {
        getEmergencyInput("Patient Name: ", newPatient.patientName, sizeof(newPatient.patientName));
        getEmergencyInput("Patient Phone: ", newPatient.patientPhone, sizeof(newPatient.patientPhone));
        newPatient.patientId = generateTempPatientId();
        printf("Assigned temporary Patient ID: %d\n", newPatient.patientId);
    }

    getEmergencyInput("Symptoms: ", newPatient.symptoms, sizeof(newPatient.symptoms));

    printf("Enter Priority (1-CRITICAL, 2-HIGH, 3-MEDIUM, 4-LOW): ");
    int priority;
    if (scanf("%d", &priority) != 1 || priority < 1 || priority > 4) {
        printf("Invalid priority. Defaulting to MEDIUM.\n");
        newPatient.priority = MEDIUM;
    } else {
        newPatient.priority = (EmergencyPriority)priority;
    }
    getchar();

    // Set arrival time and status
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(newPatient.arrivalDate, sizeof(newPatient.arrivalDate), "%d/%m/%Y", t);
    strftime(newPatient.arrivalTime, sizeof(newPatient.arrivalTime), "%H:%M", t);
    strcpy(newPatient.status, "Waiting");
    strcpy(newPatient.treatingDoctor, "N/A");
    strcpy(newPatient.treatment, "N/A");
    strcpy(newPatient.dischargeTime, "N/A");
    strcpy(newPatient.notes, "N/A");

    newPatient.emergencyId = generateEmergencyId();
    enqueueEmergencyPatient(newPatient);
    saveEmergencyRecord(&newPatient);

    printf("\nPatient added to emergency queue with ID %d.\n", newPatient.emergencyId);
    printf("Press Enter to continue...");
    getchar();
}

void viewEmergencyQueue() {
    if (isEmergencyQueueEmpty()) {
        printf("\nEmergency queue is empty.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    printf("\n==== Emergency Queue (%d patients) ====\n", emergencyQueue.count);
    printf("%-5s %-10s %-20s %-15s %-10s %-15s %-12s\n",
           "Pos", "Emerg ID", "Patient Name", "Phone", "Priority", "Arrival Time", "Status");
    printf("---------------------------------------------------------------------------------\n");

    for (int i = 0; i < emergencyQueue.count; i++) {
        const int index = (emergencyQueue.front + i) % MAX_EMERGENCY_QUEUE;
        EmergencyPatient* p = &emergencyQueue.patients[index];

        printf("%-5d %-10d %-20s %-15s %-10s %-15s %-12s\n",
               i + 1, p->emergencyId, p->patientName, p->patientPhone,
               getPriorityString(p->priority), p->arrivalTime, p->status);
    }

    printf("\nPress Enter to continue...");
    getchar();
}

void treatNextPatient() {
    if (isEmergencyQueueEmpty()) {
        printf("\nNo patients in emergency queue.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    EmergencyPatient patient = dequeueEmergencyPatient();
    char buffer[256];

    printf("\n==== Treating Patient ====\n");
    showEmergencyPatient(&patient);

    strcpy(patient.status, "In Treatment");

    getEmergencyInput("Treating Doctor: ", buffer, sizeof(buffer));
    setEmergencyOrNA(patient.treatingDoctor, buffer, sizeof(patient.treatingDoctor));

    getEmergencyInput("Treatment/Diagnosis: ", buffer, sizeof(buffer));
    setEmergencyOrNA(patient.treatment, buffer, sizeof(patient.treatment));

    // Add medicines if needed
    printf("\nWould you like to add medicines? (y/n): ");
    char choice;
    scanf("%c", &choice);
    getchar();

    if (choice == 'y' || choice == 'Y') {
        addMedicineToTreatment(&patient);
    }

    getEmergencyInput("Additional Notes: ", buffer, sizeof(buffer));
    setEmergencyOrNA(patient.notes, buffer, sizeof(patient.notes));

    // Put patient back in queue with updated status
    enqueueEmergencyPatient(patient);
    saveEmergencyRecord(&patient);

    printf("\nPatient treatment updated successfully!\n");
    printf("Press Enter to continue...");
    getchar();
}

void addMedicineToTreatment(EmergencyPatient* patient) {
    if (patient->medicineCount >= MAX_EMERGENCY_MEDICINES) {
        printf("Maximum medicine limit reached for this patient.\n");
        return;
    }

    char buffer[100];

    while (patient->medicineCount < MAX_EMERGENCY_MEDICINES) {
        printf("\n==== Add Medicine %d ====\n", patient->medicineCount + 1);

        getEmergencyInput("Medicine ID (0 to finish): ", buffer, sizeof(buffer));
        const int medicineId = atoi(buffer);

        if (medicineId == 0) break;

        const Medicine medicine = findMedicine(medicineId);
        if (medicine.medicineId == 0) {
            printf("Medicine not found in inventory!\n");
            getEmergencyInput("Medicine Name: ", buffer, sizeof(buffer));
            setEmergencyOrNA(patient->medicines[patient->medicineCount].medicineName,
                           buffer, sizeof(patient->medicines[patient->medicineCount].medicineName));
            patient->medicines[patient->medicineCount].medicineId = 0;
        } else {
            patient->medicines[patient->medicineCount].medicineId = medicine.medicineId;
            strncpy(patient->medicines[patient->medicineCount].medicineName,
                   medicine.name, sizeof(patient->medicines[patient->medicineCount].medicineName) - 1);
        }

        getEmergencyInput("Quantity: ", buffer, sizeof(buffer));
        patient->medicines[patient->medicineCount].quantity = atoi(buffer);

        getEmergencyInput("Dosage: ", buffer, sizeof(buffer));
        setEmergencyOrNA(patient->medicines[patient->medicineCount].dosage,
                       buffer, sizeof(patient->medicines[patient->medicineCount].dosage));

        getEmergencyInput("Instructions: ", buffer, sizeof(buffer));
        setEmergencyOrNA(patient->medicines[patient->medicineCount].instructions,
                       buffer, sizeof(patient->medicines[patient->medicineCount].instructions));

        patient->medicineCount++;

        printf("Add another medicine? (y/n): ");
        char cont;
        scanf("%c", &cont);
        getchar();

        if (cont != 'y' && cont != 'Y') break;
    }
}

void dischargePatient() {
    int emergencyId;
    printf("Enter Emergency ID to discharge: ");
    if (scanf("%d", &emergencyId) != 1) {
        while (getchar() != '\n'); // Clear buffer
        printf("Invalid input.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    getchar();

    FILE *fp = fopen("data/emergency_records.csv", "r");
    if (!fp) {
        printf("No emergency data found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    char emergencyLine[512];
    int found = 0;
    int lineEmergencyId, emergPatientId, priority;
    char patientName[50], phoneNumber[20], symptoms[200], arrivalDate[20], arrivalTime[10];
    char status[20], assignedDoctor[50], treatment[200], notes[200];

    while (fgets(emergencyLine, sizeof(emergencyLine), fp)) {
        // Use a temporary variable for the ID from the current line
        int currentId;
        if (sscanf(emergencyLine, "%d,", &currentId) == 1 && currentId == emergencyId) {
            // If the ID matches, parse the rest of the line
            if (sscanf(emergencyLine, "%d,%d,%49[^,],%19[^,],%199[^,],%d,%19[^,],%9[^,],%19[^,],%49[^,],%199[^,],%199[^\n]",
                       &lineEmergencyId, &emergPatientId, patientName, phoneNumber, symptoms, &priority,
                       arrivalDate, arrivalTime, status, assignedDoctor, treatment, notes) == 12) {
                found = 1;
                break;
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("Emergency record with ID %d not found!\n", emergencyId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    if (strcmp(status, "Discharged") == 0) {
        printf("Patient already discharged!\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    // Check if patient exists in main system using existing findPatient function
    char patientIdStr[10];
    sprintf(patientIdStr, "%d", emergPatientId);
    Patient existingPatient = findPatientBySearch(1, patientIdStr, NULL); // Search by ID
    int patientAdded = 0;

    if (existingPatient.patientId == 0) {
        printf("Patient not found in main system. Adding patient information...\n\n");

        // Create new patient from emergency data
        Patient newPatient = {0};
        newPatient.patientId = emergPatientId;
        strcpy(newPatient.name, patientName);
        strcpy(newPatient.phone, phoneNumber);

        // Get additional required information
        char buffer[256];
        printf("Enter additional patient information:\n");

        printf("Age: ");
        scanf("%d", &newPatient.age);
        getchar();

        printf("Gender (M/F): ");
        scanf(" %c", &newPatient.gender);
        getchar();

        printf("Address: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(newPatient.address, buffer);

        printf("Email (optional): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0) {
            strcpy(newPatient.email, buffer);
        } else {
            strcpy(newPatient.email, "N/A");
        }

        printf("Blood Type (optional): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0) {
            strcpy(newPatient.bloodType, buffer);
        } else {
            strcpy(newPatient.bloodType, "N/A");
        }

        printf("Known Allergies (optional): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) > 0) {
            strcpy(newPatient.allergies, buffer);
        } else {
            strcpy(newPatient.allergies, "N/A");
        }

        printf("Emergency Contact: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(newPatient.emergencyContact, buffer);

        printf("Primary Doctor: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        strcpy(newPatient.primaryDoctor, buffer);

        // Save the new patient using existing function
        makePatientEntry(&newPatient);
        patientAdded = 1;

        printf("\nPress Enter to continue with discharge...");
        getchar();
    } else {
        printf("Patient found in system: %s (ID: %d)\n", existingPatient.name, existingPatient.patientId);
    }

    // Get discharge information
    char dischargeNotes[300];
    char finalTreatment[200];
    char followUpRequired;

    printf("\n==== Discharge Information ====\n");
    printf("Patient: %s (ID: %d)\n", patientName, emergPatientId);
    printf("Original Symptoms: %s\n", symptoms);
    printf("Treatment Given: %s\n", treatment);

    printf("\nEnter discharge details:\n");
    printf("Final Treatment Notes: ");
    fgets(finalTreatment, sizeof(finalTreatment), stdin);
    finalTreatment[strcspn(finalTreatment, "\n")] = 0;

    printf("Discharge Notes: ");
    fgets(dischargeNotes, sizeof(dischargeNotes), stdin);
    dischargeNotes[strcspn(dischargeNotes, "\n")] = 0;

    printf("Follow-up appointment required? (y/n): ");
    scanf(" %c", &followUpRequired);
    getchar();

    // Update emergency record to discharged
    FILE *tempFp = fopen("data/emergency_temp.csv", "w");
    fp = fopen("data/emergency_records.csv", "r");

    if (!fp || !tempFp) {
        printf("Error updating emergency record.\n");
        if (fp) fclose(fp);
        if (tempFp) fclose(tempFp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        int lineEmergencyId;
        if (sscanf(line, "%d,", &lineEmergencyId) == 1 && lineEmergencyId == emergencyId) {
            // Update this record with discharge information
            fprintf(tempFp, "%d,%d,%s,%s,%s,%d,%s,%s,Discharged,%s,%s,%s\n",
                    emergencyId, emergPatientId, patientName, phoneNumber, symptoms,
                    priority, arrivalDate, arrivalTime, assignedDoctor, finalTreatment, dischargeNotes);
        } else {
            fputs(line, tempFp);
        }
    }

    fclose(fp);
    fclose(tempFp);

    // Replace original file
    remove("data/emergency_records.csv");
    rename("data/emergency_temp.csv", "data/emergency_records.csv");

    // Create follow-up appointment if required
    if (followUpRequired == 'y' || followUpRequired == 'Y') {
        printf("\n==== Creating Follow-up Appointment ====\n");

        char appointmentDate[20];
        char appointmentTime[10];
        char appointmentPurpose[200];
        char appointmentDoctor[50];

        printf("Enter appointment details:\n");
        printf("Appointment Date (DD/MM/YYYY): ");
        fgets(appointmentDate, sizeof(appointmentDate), stdin);
        appointmentDate[strcspn(appointmentDate, "\n")] = 0;

        printf("Appointment Time (HH:MM): ");
        fgets(appointmentTime, sizeof(appointmentTime), stdin);
        appointmentTime[strcspn(appointmentTime, "\n")] = 0;

        printf("Doctor Name [%s]: ", assignedDoctor);
        fgets(appointmentDoctor, sizeof(appointmentDoctor), stdin);
        appointmentDoctor[strcspn(appointmentDoctor, "\n")] = 0;
        if (strlen(appointmentDoctor) == 0) {
            strcpy(appointmentDoctor, assignedDoctor);
        }

        // Create appointment purpose based on emergency details
        snprintf(appointmentPurpose, sizeof(appointmentPurpose),
                 "Follow-up for Emergency ID: %d - %s", emergencyId, symptoms);

        // Create and save appointment
        Appointment appointment = {0};
        appointment.appointmentId = generateAppointmentId();
        appointment.patientId = emergPatientId;
        strcpy(appointment.doctorName, appointmentDoctor);
        strcpy(appointment.date, appointmentDate);
        strcpy(appointment.time, appointmentTime);
        strcpy(appointment.purpose, appointmentPurpose);
        strcpy(appointment.status, "Scheduled");

        // Save appointment to file
        FILE *appointmentFp = fopen("data/appointment.csv", "a");
        if (appointmentFp) {
            fprintf(appointmentFp, "%d,%d,%s,%s,%s,%s,%s\n",
                    appointment.appointmentId, appointment.patientId,
                    appointment.doctorName, appointment.date,
                    appointment.time, appointment.purpose,
                    appointment.status);
            fclose(appointmentFp);

            printf("\nFollow-up appointment created successfully!\n");
            printf("Appointment ID: %d\n", appointment.appointmentId);
        } else {
            printf("Error creating appointment file.\n");
        }
    }

    printf("\nPatient discharged successfully!\n");
    printf("Emergency ID: %d\n", emergencyId);
    printf("Patient: %s (ID: %d)\n", patientName, emergPatientId);
    printf("Status: Discharged\n");

    if (patientAdded) {
        printf("✓ Patient added to main system\n");
    }
    if (followUpRequired == 'y' || followUpRequired == 'Y') {
        printf("✓ Follow-up appointment scheduled\n");
    }

    printf("\nPress Enter to return to menu...");
    getchar();
}

void scheduleFollowUpAppointment(const EmergencyPatient* patient) {
    if (patient->patientId == 0) {
        printf("Patient must be registered in the system first to schedule appointment.\n");
        return;
    }

    char buffer[100];

    getEmergencyInput("Follow-up Doctor: ", buffer, sizeof(buffer));
    if (isEmergencyEffectivelyEmpty(buffer)) {
        strcpy(buffer, patient->treatingDoctor);
    }

    char date[20], time[10];
    getEmergencyInput("Appointment Date (DD/MM/YYYY): ", date, sizeof(date));
    getEmergencyInput("Appointment Time (HH:MM): ", time, sizeof(time));

    Appointment followUp = makeAppointment(patient->patientId, buffer, date, time, "Follow-up after emergency treatment");
    makeAppointmentEntry(&followUp);

    printf("Follow-up appointment scheduled successfully!\n");
}

void showEmergencyPatient(EmergencyPatient* patient) {
    printf("\n==== Emergency Patient Details ====\n");
    printf("Emergency ID: %d\n", patient->emergencyId);
    printf("Patient ID: %d\n", patient->patientId);
    printf("Name: %s\n", patient->patientName);
    printf("Phone: %s\n", patient->patientPhone);
    printf("Symptoms: %s\n", patient->symptoms);
    printf("Priority: %s\n", getPriorityString(patient->priority));
    printf("Arrival: %s %s\n", patient->arrivalDate, patient->arrivalTime);
    printf("Status: %s\n", patient->status);
    printf("Treating Doctor: %s\n", patient->treatingDoctor);
    printf("Treatment: %s\n", patient->treatment);

    if (patient->medicineCount > 0) {
        printf("\nMedicines Prescribed:\n");
        for (int i = 0; i < patient->medicineCount; i++) {
            printf("  %d. %s - Qty: %d, Dosage: %s\n",
                   i + 1, patient->medicines[i].medicineName,
                   patient->medicines[i].quantity, patient->medicines[i].dosage);
        }
    }

    if (strcmp(patient->dischargeTime, "N/A") != 0) {
        printf("Discharge Time: %s\n", patient->dischargeTime);
    }
    printf("Notes: %s\n", patient->notes);
    printf("====================================\n");
}

void saveEmergencyRecord(EmergencyPatient* patient) {
    // Part 1: Save/Update the main emergency record in emergency_records.csv
    FILE *fp = fopen(EMERGENCY_DATAFILE, "r");
    FILE *tempFp = fopen("data/emergency_temp.csv", "w");

    if (!tempFp) {
        perror("Unable to create temporary emergency file");
        if (fp) fclose(fp);
        return;
    }

    int recordExists = 0;
    if (fp) {
        char line[1024];
        while (fgets(line, sizeof(line), fp)) {
            int currentId;
            if (sscanf(line, "%d,", &currentId) == 1 && currentId == patient->emergencyId) {
                // This is the record to update, so write the new data
                fprintf(tempFp, "%d,%d,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s\n",
                        patient->emergencyId, patient->patientId, patient->patientName,
                        patient->patientPhone, patient->symptoms, patient->priority,
                        patient->arrivalDate, patient->arrivalTime, patient->status,
                        patient->treatingDoctor, patient->treatment, patient->dischargeTime,
                        patient->notes);
                recordExists = 1;
            } else {
                // Copy other lines as they are
                fputs(line, tempFp);
            }
        }
        fclose(fp);
    }

    // If the record was not found in the file, append it
    if (!recordExists) {
        fprintf(tempFp, "%d,%d,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s\n",
                patient->emergencyId, patient->patientId, patient->patientName,
                patient->patientPhone, patient->symptoms, patient->priority,
                patient->arrivalDate, patient->arrivalTime, patient->status,
                patient->treatingDoctor, patient->treatment, patient->dischargeTime,
                patient->notes);
    }

    fclose(tempFp);

    // Replace the old file with the updated one
    remove(EMERGENCY_DATAFILE);
    rename("data/emergency_temp.csv", EMERGENCY_DATAFILE);


    // Part 2: Save the medicine records to emergency_medicine.csv
    if (patient->medicineCount > 0) {
        FILE *medFp = fopen(EMERGENCY_MEDICINE_DATAFILE, "a");
        if (!medFp) {
            perror("Unable to open emergency medicine data file");
            return;
        }

        for (int i = 0; i < patient->medicineCount; i++) {
            EmergencyMedicine *med = &patient->medicines[i];
            fprintf(medFp, "%d,%d,%s,%d,%s,%s\n",
                    patient->emergencyId,
                    med->medicineId,
                    med->medicineName,
                    med->quantity,
                    med->dosage,
                    med->instructions);
        }
        fclose(medFp);
    }
}

void emergencyPatientQueue() {
    initializeEmergencyQueue();
    int choice;

    while (1) {
        system("cls");
        printf("==== Emergency Patient Queue ====\n\n");
        printf("Queue Status: %d/%d patients\n", emergencyQueue.count, MAX_EMERGENCY_QUEUE);
        printf("\n1. Add Emergency Patient\n");
        printf("2. View Emergency Queue\n");
        printf("3. Treat Next Patient\n");
        printf("4. Discharge Patient\n");
        printf("5. Search Emergency Patient\n");
        printf("6. Generate Emergency Report\n");
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
                addPatientToEmergencyQueue();
                break;
            case 2:
                viewEmergencyQueue();
                break;
            case 3:
                treatNextPatient();
                break;
            case 4:
                dischargePatient();
                break;
            case 5:
                searchEmergencyPatient();
                break;
            case 6:
                generateEmergencyReport();
                break;
            case 7:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}

void searchEmergencyPatient() {
    if (isEmergencyQueueEmpty()) {
        printf("\nNo patients in emergency queue.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    int emergencyId;
    printf("Enter Emergency ID to search: ");
    scanf("%d", &emergencyId);
    getchar();

    for (int i = 0; i < emergencyQueue.count; i++) {
        const int index = (emergencyQueue.front + i) % MAX_EMERGENCY_QUEUE;
        if (emergencyQueue.patients[index].emergencyId == emergencyId) {
            showEmergencyPatient(&emergencyQueue.patients[index]);
            printf("Press Enter to continue...");
            getchar();
            return;
        }
    }

    printf("Emergency patient with ID %d not found in queue.\n", emergencyId);
    printf("Press Enter to continue...");
    getchar();
}

void generateEmergencyReport() {
    printf("\n==== Emergency Department Report ====\n");
    printf("Current Queue Status: %d patients\n", emergencyQueue.count);

    if (emergencyQueue.count == 0) {
        printf("No patients currently in emergency queue.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    int priorities[5] = {0}; // Count for each priority level
    int waiting = 0, inTreatment = 0;

    for (int i = 0; i < emergencyQueue.count; i++) {
        const int index = (emergencyQueue.front + i) % MAX_EMERGENCY_QUEUE;
        const EmergencyPatient* p = &emergencyQueue.patients[index];

        priorities[p->priority]++;

        if (strcmp(p->status, "Waiting") == 0) waiting++;
        else if (strcmp(p->status, "In Treatment") == 0) inTreatment++;
    }

    printf("\nPriority Breakdown:\n");
    printf("CRITICAL: %d patients\n", priorities[CRITICAL]);
    printf("HIGH: %d patients\n", priorities[HIGH]);
    printf("MEDIUM: %d patients\n", priorities[MEDIUM]);
    printf("LOW: %d patients\n", priorities[LOW]);

    printf("\nStatus Breakdown:\n");
    printf("Waiting: %d patients\n", waiting);
    printf("In Treatment: %d patients\n", inTreatment);

    printf("\nPress Enter to continue...");
    getchar();
}