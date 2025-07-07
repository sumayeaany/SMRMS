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

static EmergencyQueue emergencyQueue;
static int maxEmergencyId = 5000;
static int isQueueInitialized = 0;

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

void getCurrentDateTime(char* date, char* timeStr) {
    time_t now;
    time(&now);
    struct tm* tm_info = localtime(&now);

    strftime(date, 11, "%d/%m/%Y", tm_info);
    strftime(timeStr, 9, "%H:%M:%S", tm_info);
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
        const Patient existingPatient = findPatient(3, idStr);

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
    EmergencyPatient patient = createEmergencyEntry();

    if (patient.emergencyId == -1) {
        printf("Failed to create emergency entry.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    enqueueEmergencyPatient(patient);
    saveEmergencyRecord(&patient);

    printf("\nPatient added to emergency queue successfully!\n");
    printf("Emergency ID: %d\n", patient.emergencyId);
    printf("Priority: %s\n", getPriorityString(patient.priority));
    printf("Current queue position: Based on priority\n");

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
    if (isEmergencyQueueEmpty()) {
        printf("\nNo patients in emergency queue.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    int emergencyId;
    printf("Enter Emergency ID to discharge: ");
    scanf("%d", &emergencyId);
    getchar();

    // Find patient in queue
    for (int i = 0; i < emergencyQueue.count; i++) {
        const int index = (emergencyQueue.front + i) % MAX_EMERGENCY_QUEUE;
        if (emergencyQueue.patients[index].emergencyId == emergencyId) {
            EmergencyPatient* patient = &emergencyQueue.patients[index];

            printf("\n==== Discharging Patient ====\n");
            showEmergencyPatient(patient);

            strcpy(patient->status, "Discharged");
            char* buffer = 0;
            getCurrentDateTime(buffer, patient->dischargeTime);

            printf("\nWould you like to schedule a follow-up appointment? (y/n): ");
            char choice;
            scanf("%c", &choice);
            getchar();

            if (choice == 'y' || choice == 'Y') {
                scheduleFollowUpAppointment(patient);
            }

            saveEmergencyRecord(patient);

            // Remove from queue by shifting elements
            for (int j = i; j < emergencyQueue.count - 1; j++) {
                const int currentIndex = (emergencyQueue.front + j) % MAX_EMERGENCY_QUEUE;
                const int nextIndex = (emergencyQueue.front + j + 1) % MAX_EMERGENCY_QUEUE;
                emergencyQueue.patients[currentIndex] = emergencyQueue.patients[nextIndex];
            }
            emergencyQueue.count--;
            emergencyQueue.rear = (emergencyQueue.rear - 1 + MAX_EMERGENCY_QUEUE) % MAX_EMERGENCY_QUEUE;

            printf("Patient discharged successfully!\n");
            printf("Press Enter to continue...");
            getchar();
            return;
        }
    }

    printf("Patient with Emergency ID %d not found in queue.\n", emergencyId);
    printf("Press Enter to continue...");
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
    FILE *fp = fopen(EMERGENCY_DATAFILE, "a");
    if (!fp) {
        fp = fopen(EMERGENCY_DATAFILE, "w");
        if (!fp) {
            perror("Unable to create emergency data file");
            return;
        }
        fclose(fp);
        fp = fopen(EMERGENCY_DATAFILE, "a");
    }

    // Save basic emergency record
    fprintf(fp, "%d,%d,%s,%s,%s,%d,%s,%s,%s,%s,%s,%s,%s\n",
            patient->emergencyId, patient->patientId, patient->patientName,
            patient->patientPhone, patient->symptoms, patient->priority,
            patient->arrivalDate, patient->arrivalTime, patient->status,
            patient->treatingDoctor, patient->treatment, patient->dischargeTime,
            patient->notes);

    fclose(fp);
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