#ifndef EMERGENCY_H
#define EMERGENCY_H

#define MAX_EMERGENCY_QUEUE 50
#define MAX_EMERGENCY_MEDICINES 10

typedef enum {
    CRITICAL = 1,
    HIGH = 2,
    MEDIUM = 3,
    LOW = 4
} EmergencyPriority;

typedef struct {
    int medicineId;
    char medicineName[50];
    int quantity;
    char dosage[30];
    char instructions[100];
} EmergencyMedicine;

typedef struct {
    int emergencyId;
    int patientId;
    char patientName[50];
    char patientPhone[15];
    char symptoms[200];
    EmergencyPriority priority;
    char arrivalTime[9];  // HH:MM:SS
    char arrivalDate[11]; // DD/MM/YYYY
    char status[20];      // "Waiting", "In Treatment", "Discharged"
    char treatingDoctor[50];
    char treatment[200];
    EmergencyMedicine medicines[MAX_EMERGENCY_MEDICINES];
    int medicineCount;
    char dischargeTime[9];
    char notes[300];
} EmergencyPatient;

typedef struct {
    EmergencyPatient patients[MAX_EMERGENCY_QUEUE];
    int front;
    int rear;
    int count;
} EmergencyQueue;

// Queue operations
void initializeEmergencyQueue();
int isEmergencyQueueEmpty();
int isEmergencyQueueFull();
void enqueueEmergencyPatient(EmergencyPatient patient);
EmergencyPatient dequeueEmergencyPatient();
void sortQueueByPriority();

// Emergency management functions
void emergencyPatientQueue();
EmergencyPatient createEmergencyEntry();
void addPatientToEmergencyQueue();
void viewEmergencyQueue();
void treatNextPatient();
void dischargePatient();
void searchEmergencyPatient();
void addMedicineToTreatment(EmergencyPatient* patient);
void scheduleFollowUpAppointment(const EmergencyPatient* patient);
void generateEmergencyReport();
void viewPatientTreatmentHistory();

// Utility functions
int generateEmergencyId();
void getCurrentDateTime(char* date, char* time);
const char* getPriorityString(EmergencyPriority priority);
void showEmergencyPatient(EmergencyPatient* patient);
void saveEmergencyRecord(EmergencyPatient* patient);

#endif //EMERGENCY_H