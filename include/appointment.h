//
// Created by User on 07-Jul-25.
//

#ifndef APPOINTMENT_H
#define APPOINTMENT_H
typedef struct {
    int appointmentId;
    int patientId;
    char doctorName[50];
    char date[12];        // DD/MM/YYYY
    char time[6];         // HH:MM
    char purpose[100];
    char status[20];      // Scheduled, Completed, Cancelled
} Appointment;

// Function declarations
void initializeMaxAppointmentId();
int generateAppointmentId();
Appointment makeAppointment(int patientId, const char* doctorName, const char* date, const char* time, const char* purpose);
Appointment findAppointment(int appointmentId);
void showAppointment(Appointment* appointment);
void makeAppointmentEntry(Appointment* appointment);
void listAllAppointments();
void editAppointment(int appointmentId, Appointment* appointment);
void deleteAppointment(int appointmentId);
void appointmentInformationLookup();

#endif //APPOINTMENT_H