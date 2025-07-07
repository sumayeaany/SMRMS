#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "report.h"

#define REPORTS_DATAFILE "data/report.csv"

static int maxReportId = 3000;
static int isMaxReportIdInitialized = 0;

// Helper function to check if a string is effectively empty
static int isReportEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function for input
static void getReportInput(const char* prompt, char* dest, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)size, stdin)) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        dest[0] = '\0';
    }
}

// Helper: set "N/A" if input is empty
static void setReportOrNA(char* dest, const char* input, const size_t size) {
    if (!isReportEffectivelyEmpty(input))
        strncpy(dest, input, size - 1);
    else
        strncpy(dest, "N/A", size - 1);
    dest[size - 1] = '\0';
}

// Helper: Get current date and time
static void getCurrentDateTime(char* date, char* timeStr) {
    time_t now;
    time(&now);
    struct tm* tm_info = localtime(&now);

    strftime(date, 12, "%d/%m/%Y", tm_info);
    strftime(timeStr, 9, "%H:%M:%S", tm_info);
}

void initializeMaxReportId() {
    if (isMaxReportIdInitialized) return;

    FILE *fp = fopen(REPORTS_DATAFILE, "r");
    if (!fp) {
        maxReportId = 3000;
        isMaxReportIdInitialized = 1;
        return;
    }

    Report report;
    maxReportId = 3000;
    while (fscanf(fp, "%d,%29[^,],%11[^,],%9[^,],%199[^\n]",
                  &report.reportId, report.reportType, report.generatedDate,
                  report.generatedTime, report.description) == 5) {
        if (report.reportId > maxReportId) {
            maxReportId = report.reportId;
        }
    }
    fclose(fp);
    isMaxReportIdInitialized = 1;
}

int generateReportId() {
    if (!isMaxReportIdInitialized) {
        initializeMaxReportId();
    }
    return ++maxReportId;
}

void makeReportEntry(Report* report) {
    // Create data directory if it doesn't exist
    #ifdef _WIN32
        system("if not exist data mkdir data");
    #else
        system("mkdir -p data");
    #endif

    FILE *fp = fopen(REPORTS_DATAFILE, "a");
    if (!fp) {
        fp = fopen(REPORTS_DATAFILE, "w");
        if (!fp) {
            perror("Unable to create reports data file");
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }
        fclose(fp);
        fp = fopen(REPORTS_DATAFILE, "a");
    }

    if (report->reportId == 0) {
        report->reportId = generateReportId();
    }

    fprintf(fp, "%d,%s,%s,%s,%s\n",
            report->reportId, report->reportType, report->generatedDate,
            report->generatedTime, report->description);

    fclose(fp);
    printf("Report generated successfully with ID: %d\n", report->reportId);
}

Report makeReport() {
    Report report = {0};
    char buffer[256];

    printf("\n==== Generate New Report ====\n");

    // Report type
    getReportInput("Report type: ", buffer, sizeof(buffer));
    setReportOrNA(report.reportType, buffer, sizeof(report.reportType));

    // Description
    getReportInput("Description: ", buffer, sizeof(buffer));
    setReportOrNA(report.description, buffer, sizeof(report.description));

    // Auto-generate date and time
    getCurrentDateTime(report.generatedDate, report.generatedTime);

    return report;
}

Report findReport(const int reportId) {
    Report report = {0};
    FILE *fp = fopen(REPORTS_DATAFILE, "r");
    if (!fp) {
        printf("Unable to open reports data file\n");
        return report;
    }

    while (fscanf(fp, "%d,%29[^,],%11[^,],%9[^,],%199[^\n]",
                  &report.reportId, report.reportType, report.generatedDate,
                  report.generatedTime, report.description) == 5) {
        if (report.reportId == reportId) {
            fclose(fp);
            return report;
        }
    }
    fclose(fp);
    report.reportId = 0; // Not found
    return report;
}

void showReport(Report* report) {
    printf("\n==== Report Details ====\n");
    printf("Report ID: %d\n", report->reportId);
    printf("Type: %s\n", report->reportType);
    printf("Generated Date: %s\n", report->generatedDate);
    printf("Generated Time: %s\n", report->generatedTime);
    printf("Description: %s\n", report->description);
    printf("=========================\n");
}

void listAllReports() {
    FILE *fp = fopen(REPORTS_DATAFILE, "r");
    if (!fp) {
        printf("No reports data file found or unable to open file.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Report report;
    printf("\n==== All Reports ====\n");
    printf("%-6s %-20s %-12s %-10s %-30s\n", "ID", "Type", "Date", "Time", "Description");
    printf("--------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d,%29[^,],%11[^,],%9[^,],%199[^\n]",
                  &report.reportId, report.reportType, report.generatedDate,
                  report.generatedTime, report.description) == 5) {
        printf("%-6d %-20s %-12s %-10s %-30s\n",
               report.reportId, report.reportType, report.generatedDate,
               report.generatedTime, report.description);
    }
    fclose(fp);
    printf("\nPress Enter to return to menu...");
    getchar();
}

void deleteReport(const int reportId) {
    Report report = findReport(reportId);
    if (report.reportId == 0) {
        printf("Report with ID %d not found.\n", reportId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    showReport(&report);
    printf("\nAre you sure you want to delete this report? (y/n): ");
    char confirm;
    scanf("%c", &confirm);
    getchar();

    if (confirm != 'y' && confirm != 'Y') {
        printf("Delete operation cancelled.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    FILE *fp = fopen(REPORTS_DATAFILE, "r");
    FILE *temp = fopen("data/temp_reports.csv", "w");

    if (!fp || !temp) {
        printf("Error accessing files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Report currentReport;
    while (fscanf(fp, "%d,%29[^,],%11[^,],%9[^,],%199[^\n]",
                  &currentReport.reportId, currentReport.reportType, currentReport.generatedDate,
                  currentReport.generatedTime, currentReport.description) == 5) {

        if (currentReport.reportId != reportId) {
            fprintf(temp, "%d,%s,%s,%s,%s\n",
                    currentReport.reportId, currentReport.reportType, currentReport.generatedDate,
                    currentReport.generatedTime, currentReport.description);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(REPORTS_DATAFILE);
    rename("data/temp_reports.csv", REPORTS_DATAFILE);

    printf("Report deleted successfully.\n");
    printf("Press Enter to return to menu...");
    getchar();
}

void generatePatientProfileReport() {
    int patientId;
    printf("Enter Patient ID for profile report: ");
    scanf("%d", &patientId);
    getchar();

    char patientIdStr[10];
    sprintf(patientIdStr, "%d", patientId);
    Patient patient = findPatient(3, patientIdStr);

    if (patient.patientId == 0) {
        printf("Patient with ID %d not found.\n", patientId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    printf("\n==== Patient Profile Report ====\n");
    showPatient(&patient);

    Report report;
    sprintf(report.reportType, "Patient Profile");
    sprintf(report.description, "Profile report for Patient ID: %d (%s)", patient.patientId, patient.name);
    getCurrentDateTime(report.generatedDate, report.generatedTime);
    makeReportEntry(&report);

    printf("Press Enter to return to menu...");
    getchar();
}

void generatePatientAppointmentHistory() {
    int patientId;
    printf("Enter Patient ID for appointment history: ");
    scanf("%d", &patientId);
    getchar();

    char patientIdStr[10];
    sprintf(patientIdStr, "%d", patientId);
    Patient patient = findPatient(3, patientIdStr);

    if (patient.patientId == 0) {
        printf("Patient with ID %d not found.\n", patientId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    printf("\n==== Patient Appointment History ====\n");
    printf("Patient: %s (ID: %d)\n\n", patient.name, patient.patientId);

    FILE *fp = fopen("data/appointment.csv", "r");
    if (!fp) {
        printf("No appointment data found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    char line[512];
    int found = 0;
    printf("%-8s %-15s %-12s %-8s %-15s %-12s\n",
           "App ID", "Doctor", "Date", "Time", "Purpose", "Status");
    printf("------------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        const char *tok_id = strtok(lineCopy, ",");
        const char *tok_patientId = strtok(NULL, ",");

        if (tok_patientId && atoi(tok_patientId) == patientId) {
            strcpy(lineCopy, line);
            const char *tok_id2 = strtok(lineCopy, ",");
            const char *tok_patientId2 = strtok(NULL, ",");
            const char *tok_doctor = strtok(NULL, ",");
            const char *tok_date = strtok(NULL, ",");
            const char *tok_time = strtok(NULL, ",");
            const char *tok_purpose = strtok(NULL, ",");
            const char *tok_status = strtok(NULL, "\r\n");

            printf("%-8s %-15s %-12s %-8s %-15s %-12s\n",
                   tok_id2 ? tok_id2 : "N/A",
                   tok_doctor ? tok_doctor : "N/A",
                   tok_date ? tok_date : "N/A",
                   tok_time ? tok_time : "N/A",
                   tok_purpose ? tok_purpose : "N/A",
                   tok_status ? tok_status : "N/A");
            found = 1;
        }
    }

    if (!found) {
        printf("No appointments found for this patient.\n");
    }

    fclose(fp);

    Report report;
    sprintf(report.reportType, "Appointment History");
    sprintf(report.description, "Appointment history for Patient ID: %d (%s)", patient.patientId, patient.name);
    getCurrentDateTime(report.generatedDate, report.generatedTime);
    makeReportEntry(&report);

    printf("\nPress Enter to return to menu...");
    getchar();
}

void generateDailyPatientReport() {
    char date[12];
    printf("Enter date (DD/MM/YYYY) for daily patient report: ");
    scanf("%s", date);
    getchar();

    printf("\n==== Daily Patient Report for %s ====\n", date);

    FILE *fp = fopen("data/appointment.csv", "r");
    if (!fp) {
        printf("No appointment data found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    char line[512];
    int count = 0;
    printf("%-8s %-10s %-15s %-8s %-15s\n", "App ID", "Patient ID", "Doctor", "Time", "Purpose");
    printf("----------------------------------------------------------\n");

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        const char *tok_id = strtok(lineCopy, ",");
        const char *tok_patientId = strtok(NULL, ",");
        const char *tok_doctor = strtok(NULL, ",");
        const char *tok_date = strtok(NULL, ",");
        const char *tok_time = strtok(NULL, ",");
        const char *tok_purpose = strtok(NULL, ",");

        if (tok_date && strcmp(tok_date, date) == 0) {
            printf("%-8s %-10s %-15s %-8s %-15s\n",
                   tok_id ? tok_id : "N/A",
                   tok_patientId ? tok_patientId : "N/A",
                   tok_doctor ? tok_doctor : "N/A",
                   tok_time ? tok_time : "N/A",
                   tok_purpose ? tok_purpose : "N/A");
            count++;
        }
    }

    printf("\nTotal appointments for %s: %d\n", date, count);
    fclose(fp);

    Report report;
    sprintf(report.reportType, "Daily Patient Report");
    sprintf(report.description, "Daily patient appointments for %s (%d total)", date, count);
    getCurrentDateTime(report.generatedDate, report.generatedTime);
    makeReportEntry(&report);

    printf("Press Enter to return to menu...");
    getchar();
}

void generatePatientStatisticsReport() {
    printf("\n==== Patient Statistics Report ====\n");

    FILE *fp = fopen("data/patient.csv", "r");
    if (!fp) {
        printf("No patient data found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Patient patient;
    int totalPatients = 0, maleCount = 0, femaleCount = 0;
    int ageGroups[5] = {0}; // 0-18, 19-30, 31-50, 51-70, 71+

    while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                  &patient.patientId, patient.name, &patient.age, &patient.gender,
                  patient.phone, patient.address, patient.email, patient.bloodType,
                  patient.allergies, patient.emergencyContact, patient.primaryDoctor) >= 5) {
        totalPatients++;

        if (patient.gender == 'M') maleCount++;
        else if (patient.gender == 'F') femaleCount++;

        if (patient.age <= 18) ageGroups[0]++;
        else if (patient.age <= 30) ageGroups[1]++;
        else if (patient.age <= 50) ageGroups[2]++;
        else if (patient.age <= 70) ageGroups[3]++;
        else ageGroups[4]++;
    }

    printf("Total Patients: %d\n", totalPatients);
    printf("Male Patients: %d (%.1f%%)\n", maleCount, totalPatients > 0 ? (float)maleCount/totalPatients*100 : 0);
    printf("Female Patients: %d (%.1f%%)\n", femaleCount, totalPatients > 0 ? (float)femaleCount/totalPatients*100 : 0);
    printf("\nAge Group Distribution:\n");
    printf("0-18 years: %d patients\n", ageGroups[0]);
    printf("19-30 years: %d patients\n", ageGroups[1]);
    printf("31-50 years: %d patients\n", ageGroups[2]);
    printf("51-70 years: %d patients\n", ageGroups[3]);
    printf("71+ years: %d patients\n", ageGroups[4]);

    fclose(fp);

    Report report;
    sprintf(report.reportType, "Patient Statistics");
    sprintf(report.description, "Statistical overview of %d patients", totalPatients);
    getCurrentDateTime(report.generatedDate, report.generatedTime);
    makeReportEntry(&report);

    printf("\nPress Enter to return to menu...");
    getchar();
}

void reportManagement() {
    initializeMaxReportId();
    int choice;

    while (1) {
        system("cls");
        printf("==== Medical Reports ====\n\n");
        printf("1. Patient Profile Report\n");
        printf("2. Patient Appointment History\n");
        printf("3. Daily Patient Report\n");
        printf("4. Patient Statistics Report\n");
        printf("5. View All Reports\n");
        printf("6. Delete Report\n");
        printf("7. Back to Main Menu\n");
        printf("\nChoice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n') {}
            printf("Invalid input. Press Enter to continue...");
            getchar();
            continue;
        }
        getchar(); // consume newline

        switch (choice) {
            case 1:
                generatePatientProfileReport();
                break;
            case 2:
                generatePatientAppointmentHistory();
                break;
            case 3:
                generateDailyPatientReport();
                break;
            case 4:
                generatePatientStatisticsReport();
                break;
            case 5:
                listAllReports();
                break;
            case 6: {
                int reportId;
                printf("Enter Report ID to delete: ");
                scanf("%d", &reportId);
                getchar();
                deleteReport(reportId);
                break;
            }
            case 7:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}