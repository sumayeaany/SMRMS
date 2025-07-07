#ifndef REPORTS_H
#define REPORTS_H

#include "patient.h"
#include "appointment.h"
#include "medicine.h"

typedef struct {
    int reportId;
    char reportType[30];
    char generatedDate[12];
    char generatedTime[10];
    char description[200];
} Report;

// Function declarations
void reportManagement();
Report makeReport();
void makeReportEntry(Report* report);
Report findReport(const int reportId);
void showReport(Report* report);
void listAllReports();
void deleteReport(const int reportId);

// Patient-focused report functions
void generatePatientProfileReport();
void generatePatientAppointmentHistory();
void generatePatientMedicationReport();
void generateDailyPatientReport();
void generatePatientStatisticsReport();
void generatePatientsByDoctorReport();
void generatePatientsByAgeGroupReport();
void searchPatientReports();

int generateReportId();
void initializeMaxReportId();

#endif