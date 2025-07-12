#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "report.h"

#include "medicine.h"
#include "patient.h"
#include "prescription.h"

#define REPORT_DATAFILE "data/reports.csv"
#define PRESCRIPTION_DATAFILE "data/prescription.csv"
#define BILL_DATAFILE "data/bills.csv"
#define EMERGENCY_DATAFILE "data/emergency.csv"
#define EMERGENCY_MEDICINES_FILE "data/emergency_medicines.csv"
#define APPOINTMENT_DATAFILE "data/appointment.csv"

static int maxReportId = 3000;
static int maxBillId = 5000;

#define APPOINTMENT_FEE 500.00
#define EMERGENCY_BASE_FEE 200.00

// Helper functions
static void createReportsDirectory() {
    #ifdef _WIN32
        system("if not exist reports mkdir reports");
    #else
        system("mkdir -p reports");
    #endif
}
void initializeDataFiles() {
    // Create data directory if it doesn't exist
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif

    // List of all data files used in the application
    const char* dataFiles[] = {
        APPOINTMENT_DATAFILE,
        REPORT_DATAFILE,
        PRESCRIPTION_DATAFILE,
        BILL_DATAFILE,
        EMERGENCY_DATAFILE,
        EMERGENCY_MEDICINES_FILE
    };
    int numFiles = sizeof(dataFiles) / sizeof(dataFiles[0]);

    for (int i = 0; i < numFiles; ++i) {
        FILE *fp = fopen(dataFiles[i], "a"); // Open in append mode to create if not exists
        if (fp) {
            fclose(fp);
        } else {
            printf("Warning: Could not create or open data file: %s\n", dataFiles[i]);
        }
    }
}

int generateReportId() {
    return ++maxReportId;
}


int generateBillId() {
    return ++maxBillId;
}

void saveReport(Report* report) {
    createReportsDirectory();

    FILE *fp = fopen(REPORT_DATAFILE, "a");
    if (!fp) {
        printf("Error saving report.\n");
        return;
    }

    time_t now;
    time(&now);
    const struct tm *timeinfo = localtime(&now);
    strftime(report->generatedDate, sizeof(report->generatedDate), "%d/%m/%Y", timeinfo);

    fprintf(fp, "%d,%d,%s,%s,%s\n",
            report->reportId, report->type, report->generatedDate,
            report->title, report->content);

    fclose(fp);
}

void saveBill(Bill* bill) {
    createReportsDirectory();

    FILE *fp = fopen(BILL_DATAFILE, "a");
    if (!fp) {
        printf("Error saving bill.\n");
        return;
    }

    fprintf(fp, "%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%s,%s,%s\n",
            bill->billId, bill->patientId, bill->consultationFee, bill->medicineTotal,
            bill->tax, bill->discount, bill->grandTotal, bill->billDate,
            bill->paymentStatus, bill->notes);

    fclose(fp);
}


void generateAppointmentHistoryReport() {
    int patientId;
    printf("Enter Patient ID: ");
    scanf("%d", &patientId);
    getchar();

    Report report = {0};
    report.reportId = generateReportId();
    report.type = APPOINTMENT_HISTORY;
    sprintf(report.title, "Appointment History - Patient ID: %d", patientId);

    char content[2000] = "==== APPOINTMENT HISTORY REPORT ====\n\n";
    char line[200];

    FILE *fp = fopen("data/appointment.csv", "r");
    if (!fp) {
        strcat(content, "No appointment data found.\n");
    } else {
        int appointmentCount = 0;
        char buffer[512];

        while (fgets(buffer, sizeof(buffer), fp)) {
            int apptId, patId;
            char doctor[50], date[20], time[10], purpose[100], status[20];

            if (sscanf(buffer, "%d,%d,%49[^,],%19[^,],%9[^,],%99[^,],%19[^\n]",
                      &apptId, &patId, doctor, date, time, purpose, status) == 7) {

                if (patId == patientId) {
                    sprintf(line, "Appointment ID: %d\n", apptId);
                    strcat(content, line);
                    sprintf(line, "Doctor: %s\n", doctor);
                    strcat(content, line);
                    sprintf(line, "Date: %s\n", date);
                    strcat(content, line);
                    sprintf(line, "Time: %s\n", time);
                    strcat(content, line);
                    sprintf(line, "Purpose: %s\n", purpose);
                    strcat(content, line);
                    sprintf(line, "Status: %s\n\n", status);
                    strcat(content, line);
                    appointmentCount++;
                }
            }
        }
        fclose(fp);

        sprintf(line, "Total Appointments: %d\n", appointmentCount);
        strcat(content, line);
    }

    strcpy(report.content, content);
    saveReport(&report);

    printf("\n%s", content);
    printf("\nReport saved with ID: %d\n", report.reportId);
    printf("Press Enter to return to menu...");
    getchar();
}

void generateDailyPatientReport() {
    char date[20];
    printf("Enter date (DD/MM/YYYY): ");
    fgets(date, sizeof(date), stdin);
    date[strcspn(date, "\n")] = 0;

    Report report = {0};
    report.reportId = generateReportId();
    report.type = DAILY_PATIENT;
    sprintf(report.title, "Daily Patient Report - %s", date);

    char content[2000];
    sprintf(content, "==== DAILY PATIENT REPORT ====\nDate: %s\n\n", date);
    char line[200];

    FILE *fp = fopen("data/appointment.csv", "r");
    if (!fp) {
        strcat(content, "No appointment data found.\n");
    } else {
        int patientCount = 0;
        char buffer[512];

        while (fgets(buffer, sizeof(buffer), fp)) {
            int apptId, patId;
            char doctor[50], apptDate[20], time[10], purpose[100], status[20];

            if (sscanf(buffer, "%d,%d,%49[^,],%19[^,],%9[^,],%99[^,],%19[^\n]",
                      &apptId, &patId, doctor, apptDate, time, purpose, status) == 7) {

                if (strcmp(apptDate, date) == 0) {
                    sprintf(line, "Patient ID: %d | Doctor: %s | Time: %s | Purpose: %s\n",
                           patId, doctor, time, purpose);
                    strcat(content, line);
                    patientCount++;
                }
            }
        }
        fclose(fp);

        sprintf(line, "\nTotal Patients: %d\n", patientCount);
        strcat(content, line);
    }

    strcpy(report.content, content);
    saveReport(&report);

    printf("\n%s", content);
    printf("\nReport saved with ID: %d\n", report.reportId);
    printf("Press Enter to return to menu...");
    getchar();
}

void generatePatientStatisticsReport() {
    Report report = {0};
    report.reportId = generateReportId();
    report.type = PATIENT_STATISTICS;
    strcpy(report.title, "Patient Statistics Report");

    char content[2000] = "==== PATIENT STATISTICS REPORT ====\n\n";
    char line[200];

    FILE *fp = fopen("data/patient.csv", "r");
    if (!fp) {
        strcat(content, "No patient data found.\n");
    } else {
        int totalPatients = 0;
        int maleCount = 0, femaleCount = 0;
        int ageGroups[5] = {0}; // 0-18, 19-30, 31-50, 51-70, 70+

        Patient patient;
        while (fscanf(fp, "%d,%49[^,],%d,%c,%14[^,],%99[^,],%49[^,],%4[^,],%199[^,],%14[^,],%49[^\n]",
                      &patient.patientId, patient.name, &patient.age, &patient.gender,
                      patient.phone, patient.address, patient.email, patient.bloodType,
                      patient.allergies, patient.emergencyContact, patient.primaryDoctor) >= 5) {
            totalPatients++;

            if (patient.gender == 'M') maleCount++;
            else femaleCount++;

            if (patient.age <= 18) ageGroups[0]++;
            else if (patient.age <= 30) ageGroups[1]++;
            else if (patient.age <= 50) ageGroups[2]++;
            else if (patient.age <= 70) ageGroups[3]++;
            else ageGroups[4]++;
        }
        fclose(fp);

        sprintf(line, "Total Patients: %d\n", totalPatients);
        strcat(content, line);
        sprintf(line, "Male Patients: %d (%.1f%%)\n", maleCount,
                totalPatients > 0 ? (maleCount * 100.0 / totalPatients) : 0);
        strcat(content, line);
        sprintf(line, "Female Patients: %d (%.1f%%)\n\n", femaleCount,
                totalPatients > 0 ? (femaleCount * 100.0 / totalPatients) : 0);
        strcat(content, line);

        strcat(content, "Age Distribution:\n");
        sprintf(line, "0-18 years: %d\n", ageGroups[0]);
        strcat(content, line);
        sprintf(line, "19-30 years: %d\n", ageGroups[1]);
        strcat(content, line);
        sprintf(line, "31-50 years: %d\n", ageGroups[2]);
        strcat(content, line);
        sprintf(line, "51-70 years: %d\n", ageGroups[3]);
        strcat(content, line);
        sprintf(line, "70+ years: %d\n", ageGroups[4]);
        strcat(content, line);
    }

    strcpy(report.content, content);
    saveReport(&report);

    printf("\n%s", content);
    printf("\nReport saved with ID: %d\n", report.reportId);
    printf("Press Enter to return to menu...");
    getchar();
}

void generateBillingReport() {
    int patientId;
    char patientIdStr[12];
    double totalBill = 0.0;
    double appointmentCharges = 0.0;
    double emergencyCharges = 0.0;
    double medicineCharges = 0.0;

    system("cls");
    printf("==== Generate Billing Report ====\n\n");
    printf("Enter Patient ID: ");
    if (scanf("%d", &patientId) != 1) {
        while (getchar() != '\n'); // Clear buffer
        printf("Invalid input. Please enter a numeric ID.\n");
        printf("\nPress Enter to return to menu...");
        getchar();
        return;
    }
    getchar(); // Consume newline

    sprintf(patientIdStr, "%d", patientId);
    Patient patient = findPatientBySearch(1, patientIdStr, NULL);

    if (patient.patientId == 0) {
        printf("Patient with ID %d not found.\n", patientId);
        printf("\nPress Enter to return to menu...");
        getchar();
        return;
    }

    printf("\n--- Generating Report for Patient ---\n");
    printf("ID:   %d\n", patient.patientId);
    printf("Name: %s\n", patient.name);
    printf("-------------------------------------\n\n");
    printf("Itemized Bill:\n");
    printf("--------------------------------------------------------------------------\n");
    printf("%-30s %-20s %15s\n", "Description", "Date/Time", "Cost (BDT)");
    printf("--------------------------------------------------------------------------\n");

    // 1. Calculate Appointment Charges
    FILE *appFp = fopen(APPOINTMENT_DATAFILE, "r");
    if (appFp) {
        char line[512];
        while (fgets(line, sizeof(line), appFp)) {
            int appId, appPatientId;
            char doctorName[50], date[20], time[10];
            if (sscanf(line, "%d,%d,%49[^,],%19[^,],%9[^,]", &appId, &appPatientId, doctorName, date, time) >= 5) {
                if (appPatientId == patientId) {
                    char description[100];
                    snprintf(description, sizeof(description), "Appointment (Dr. %s)", doctorName);
                    char dateTime[30];
                    snprintf(dateTime, sizeof(dateTime), "%s %s", date, time);
                    printf("%-30s %-20s %15.2f\n", description, dateTime, APPOINTMENT_FEE);
                    appointmentCharges += APPOINTMENT_FEE;
                }
            }
        }
        fclose(appFp);
    }

    // 2. Calculate Prescription Medicine Charges
    FILE *prescFp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (prescFp) {
        Prescription p;
        while (fscanf(prescFp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%*s",
                      &p.prescriptionId, &p.patientId, &p.medicineId, p.medicineName,
                      &p.quantity, &p.unitPrice, &p.totalPrice, p.prescribedDate) == 8) {
            if (p.patientId == patientId) {
                char description[100];
                snprintf(description, sizeof(description), "Prescription: %s (x%d)", p.medicineName, p.quantity);
                printf("%-30s %-20s %15.2f\n", description, p.prescribedDate, p.totalPrice);
                medicineCharges += p.totalPrice;
            }
            // Skip rest of the line
            int c;
            while ((c = fgetc(prescFp)) != '\n' && c != EOF);
        }
        fclose(prescFp);
    }


    // 3. Calculate Emergency Visit and Medicine Charges
    FILE *emergFp = fopen(EMERGENCY_DATAFILE, "r");
    if (emergFp) {
        char line[1024];
        while (fgets(line, sizeof(line), emergFp)) {
            int emergId, emergPatientId;
            char arrivalDate[20];
            if (sscanf(line, "%d,%d,%*[^,],%*[^,],%*[^,],%*d,%19[^,]", &emergId, &emergPatientId, arrivalDate) == 3) {
                if (emergPatientId == patientId) {
                    char description[100];
                    snprintf(description, sizeof(description), "Emergency Visit (ID: %d)", emergId);
                    printf("%-30s %-20s %15.2f\n", description, arrivalDate, EMERGENCY_BASE_FEE);
                    emergencyCharges += EMERGENCY_BASE_FEE;

                    FILE *medFp = fopen(EMERGENCY_MEDICINES_FILE, "r");
                    if (medFp) {
                        char medLine[256];
                        while (fgets(medLine, sizeof(medLine), medFp)) {
                            int medEmergId, medId, medQty;
                            if (sscanf(medLine, "%d,%d,%*[^,],%d", &medEmergId, &medId, &medQty) == 3) {
                                if (medEmergId == emergId) {
                                    Medicine medInfo = findMedicine(medId);
                                    if (medInfo.medicineId != 0) {
                                        double cost = medInfo.price * medQty;
                                        char medDescription[100];
                                        snprintf(medDescription, sizeof(medDescription), "  Medicine: %s (x%d)", medInfo.name, medQty);
                                        printf("%-30s %-20s %15.2f\n", medDescription, arrivalDate, cost);
                                        medicineCharges += cost;
                                    }
                                }
                            }
                        }
                        fclose(medFp);
                    }
                }
            }
        }
        fclose(emergFp);
    }

    totalBill = appointmentCharges + emergencyCharges + medicineCharges;

    printf("--------------------------------------------------------------------------\n");
    printf("%52s %15.2f\n", "Subtotal Appointments:", appointmentCharges);
    printf("%52s %15.2f\n", "Subtotal Emergency Visits:", emergencyCharges);
    printf("%52s %15.2f\n", "Subtotal Medicines:", medicineCharges);
    printf("--------------------------------------------------------------------------\n");
    printf("%52s Tk.%14.2f\n", "TOTAL DUE:", totalBill);
    printf("--------------------------------------------------------------------------\n");

    printf("\nPress Enter to return to menu...");
    getchar();
}

void viewAllReports() {
    FILE *fp = fopen(REPORT_DATAFILE, "r");
    if (!fp) {
        printf("No reports found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    printf("\n==== All Reports ====\n");
    printf("%-5s %-15s %-12s %-30s\n", "ID", "Type", "Date", "Title");
    printf("----------------------------------------------------------------\n");

    Report report;
    while (fscanf(fp, "%d,%d,%19[^,],%99[^,],%[^\n]",
                  &report.reportId, &report.type, report.generatedDate,
                  report.title, report.content) == 5) {

        const char* typeStr;
        switch(report.type) {
            case PATIENT_PROFILE: typeStr = "Patient Profile"; break;
            case APPOINTMENT_HISTORY: typeStr = "Appointment Hist"; break;
            case DAILY_PATIENT: typeStr = "Daily Patient"; break;
            case PATIENT_STATISTICS: typeStr = "Patient Stats"; break;
            case PRESCRIPTION_REPORT: typeStr = "Prescription"; break;
            case BILLING_REPORT: typeStr = "Billing"; break;
            default: typeStr = "Unknown"; break;
        }

        printf("%-5d %-15s %-12s %-30s\n",
               report.reportId, typeStr, report.generatedDate, report.title);
    }

    fclose(fp);
    printf("Press Enter to return to menu...");
    getchar();
}

void deleteReport() {
    int reportId;
    printf("Enter Report ID to delete: ");
    scanf("%d", &reportId);
    getchar();

    FILE *fp = fopen(REPORT_DATAFILE, "r");
    if (!fp) {
        printf("No reports found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    FILE *temp = fopen("data/temp_reports.csv", "w");
    if (!temp) {
        printf("Error creating temporary file.\n");
        fclose(fp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Report report;
    int found = 0;

    while (fscanf(fp, "%d,%d,%19[^,],%99[^,],%[^\n]",
                  &report.reportId, &report.type, report.generatedDate,
                  report.title, report.content) == 5) {

        if (report.reportId != reportId) {
            fprintf(temp, "%d,%d,%s,%s,%s\n",
                    report.reportId, report.type, report.generatedDate,
                    report.title, report.content);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove(REPORT_DATAFILE);
        rename("data/temp_reports.csv", REPORT_DATAFILE);
        printf("Report deleted successfully.\n");
    } else {
        remove("data/temp_reports.csv");
        printf("Report with ID %d not found.\n", reportId);
    }

    printf("Press Enter to return to menu...");
    getchar();
}

static void printPrescriptionHistory(int patientId, FILE* reportFp) {
    fprintf(reportFp, "----------------------------------------\n");
    fprintf(reportFp, "       PRESCRIPTION HISTORY\n");
    fprintf(reportFp, "----------------------------------------\n\n");

    FILE *prescFp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (!prescFp) {
        fprintf(reportFp, "Could not open prescription data file.\n\n");
        return;
    }

    char line[1024];
    int prescriptionsFound = 0;
    while (fgets(line, sizeof(line), prescFp)) {
        int recordPatientId;
        // Use sscanf to check the patient ID without modifying the line
        if (sscanf(line, "%*d,%d,", &recordPatientId) == 1 && recordPatientId == patientId) {
            prescriptionsFound = 1;
            char *id = strtok(line, ",");
            strtok(NULL, ","); // patientId
            char *medId = strtok(NULL, ",");
            char *medName = strtok(NULL, ",");
            strtok(NULL, ","); // quantity
            strtok(NULL, ","); // unitPrice
            char *totalPrice = strtok(NULL, ",");
            char *date = strtok(NULL, ",");
            char *doctor = strtok(NULL, ",");
            char *dosage = strtok(NULL, ",");
            char *duration = strtok(NULL, ",");
            char *notes = strtok(NULL, "\n");

            fprintf(reportFp, "Prescription ID: %s (Medicine: %s)\n", id, medName);
            fprintf(reportFp, "  Prescribed by: Dr. %s on %s\n", doctor, date);
            fprintf(reportFp, "  Dosage: %s\n", dosage);
            fprintf(reportFp, "  Duration: %s\n", duration);
            fprintf(reportFp, "  Total Price: %.2f\n", atof(totalPrice));
            fprintf(reportFp, "  Notes: %s\n\n", notes ? notes : "N/A");
        }
    }

    if (!prescriptionsFound) {
        fprintf(reportFp, "No prescription history found.\n\n");
    }
    fclose(prescFp);
}

void generatePatientProfileReport() {
    int patientId;
    printf("Enter Patient ID to generate report for: ");
    if (scanf("%d", &patientId) != 1) {
        while (getchar() != '\n'); // Clear buffer
        printf("Invalid input.\nPress Enter to continue...");
        getchar();
        return;
    }
    getchar(); // Consume newline

    createReportsDirectory();

    char patientIdStr[12];
    sprintf(patientIdStr, "%d", patientId);
    Patient patient = findPatientBySearch(1, patientIdStr, NULL);

    if (patient.patientId == 0) {
        printf("Patient with ID %d not found.\n", patientId);
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    char filename[100];
    sprintf(filename, "reports/Patient_Profile_%d_%s.txt", patient.patientId, patient.name);

    FILE *reportFp = fopen(filename, "w");
    if (!reportFp) {
        perror("Error creating report file");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    printf("Generating report for %s (ID: %d)...\n", patient.name, patient.patientId);

    // --- Patient Demographics ---
    fprintf(reportFp, "========================================\n");
    fprintf(reportFp, "      PATIENT PROFILE REPORT\n");
    fprintf(reportFp, "========================================\n\n");
    fprintf(reportFp, "Patient ID: %d\n", patient.patientId);
    fprintf(reportFp, "Name: %s\n", patient.name);
    fprintf(reportFp, "Age: %d\n", patient.age);
    fprintf(reportFp, "Gender: %c\n", patient.gender);
    fprintf(reportFp, "Phone: %s\n", patient.phone);
    fprintf(reportFp, "Address: %s\n", patient.address);
    fprintf(reportFp, "Email: %s\n", patient.email);
    fprintf(reportFp, "Emergency Contact: %s\n\n", patient.emergencyContact);

    // --- Medical Information ---
    fprintf(reportFp, "----------------------------------------\n");
    fprintf(reportFp, "        MEDICAL INFORMATION\n");
    fprintf(reportFp, "----------------------------------------\n\n");
    fprintf(reportFp, "Blood Type: %s\n", patient.bloodType);
    fprintf(reportFp, "Known Allergies: %s\n", patient.allergies);
    fprintf(reportFp, "Primary Doctor: %s\n\n", patient.primaryDoctor);

    // --- Prescription History ---
    printPrescriptionHistory(patient.patientId, reportFp);

    // --- Emergency Visit History ---
    printEmergencyHistory(patient.patientId, reportFp);

    // --- Appointment History ---
    fprintf(reportFp, "----------------------------------------\n");
    fprintf(reportFp, "        APPOINTMENT HISTORY\n");
    fprintf(reportFp, "----------------------------------------\n\n");

    FILE *appointmentFp = fopen(APPOINTMENT_DATAFILE, "r");
    if (appointmentFp) {
        char line[512];
        int appointmentsFound = 0;
        while (fgets(line, sizeof(line), appointmentFp)) {
            int appPatientId;
            if (sscanf(line, "%*d,%d,", &appPatientId) == 1 && appPatientId == patient.patientId) {
                char *id = strtok(line, ",");
                strtok(NULL, ","); // patientId
                char *doctor = strtok(NULL, ",");
                char *date = strtok(NULL, ",");
                char *time = strtok(NULL, ",");
                char *purpose = strtok(NULL, ",");
                char *status = strtok(NULL, "\n");
                fprintf(reportFp, "Appointment ID: %s\n", id);
                fprintf(reportFp, "  Date: %s at %s\n", date, time);
                fprintf(reportFp, "  Doctor: %s\n", doctor);
                fprintf(reportFp, "  Purpose: %s\n", purpose);
                fprintf(reportFp, "  Status: %s\n\n", status);
                appointmentsFound = 1;
            }
        }
        if (!appointmentsFound) {
            fprintf(reportFp, "No appointment history found.\n\n");
        }
        fclose(appointmentFp);
    } else {
        fprintf(reportFp, "Could not open appointment data file.\n\n");
    }

    fprintf(reportFp, "========================================\n");
    fprintf(reportFp, "           END OF REPORT\n");
    fprintf(reportFp, "========================================\n");

    fclose(reportFp);
    printf("Report '%s' generated successfully.\n", filename);
    printf("Press Enter to continue...");
    getchar();
}

static void printEmergencyHistory(int patientId, FILE* reportFp) {
    fprintf(reportFp, "----------------------------------------\n");
    fprintf(reportFp, "       EMERGENCY VISIT HISTORY\n");
    fprintf(reportFp, "----------------------------------------\n\n");

    FILE *emergencyFp = fopen(EMERGENCY_DATAFILE, "r");
    if (!emergencyFp) {
        fprintf(reportFp, "No emergency records data file found.\n\n");
        return;
    }

    char line[1024];
    int visitsFound = 0;
    while (fgets(line, sizeof(line), emergencyFp)) {
        int recordPatientId, emergencyId;
        // Create a copy for sscanf to avoid modifying the original line
        char lineCopyForSscanf[1024];
        strcpy(lineCopyForSscanf, line);

        if (sscanf(lineCopyForSscanf, "%d,%d,", &emergencyId, &recordPatientId) == 2 && recordPatientId == patientId) {
            visitsFound = 1;
            char *fields[13] = {0};
            // Create a copy for strtok to parse
            char lineCopyForStrtok[1024];
            strcpy(lineCopyForStrtok, line);
            char *token = strtok(lineCopyForStrtok, ",\n");
            int i = 0;
            while(token != NULL && i < 13) {
                fields[i++] = token;
                token = strtok(NULL, ",\n");
            }

            fprintf(reportFp, "Emergency ID: %s\n", fields[0]);
            fprintf(reportFp, "  Arrival: %s at %s\n", fields[6], fields[7]);
            fprintf(reportFp, "  Symptoms: %s\n", fields[4]);
            fprintf(reportFp, "  Doctor: %s\n", fields[9]);
            fprintf(reportFp, "  Treatment: %s\n", fields[10]);
            fprintf(reportFp, "  Status: %s\n", fields[8]);
            fprintf(reportFp, "  Notes: %s\n", fields[12] ? fields[12] : "N/A");

            // Now find and print medicines for this emergency ID
            FILE *medFp = fopen(EMERGENCY_MEDICINES_FILE, "r");
            if (medFp) {
                fprintf(reportFp, "  Medicines Prescribed:\n");
                char medLine[512];
                int medsFound = 0;
                while (fgets(medLine, sizeof(medLine), medFp)) {
                    int medEmergencyId;
                    if (sscanf(medLine, "%d,", &medEmergencyId) == 1 && medEmergencyId == emergencyId) {
                        char *medFields[6] = {0};
                        // Create a copy for strtok
                        char medLineCopy[512];
                        strcpy(medLineCopy, medLine);
                        char *medToken = strtok(medLineCopy, ",\n");
                        int j = 0;
                        while(medToken != NULL && j < 6) {
                            medFields[j++] = medToken;
                            medToken = strtok(NULL, ",\n");
                        }
                        // Format: EmergID,MedID,MedName,Qty,Dosage,Instructions
                        fprintf(reportFp, "    - %s (ID: %s): Qty: %s, Dosage: %s, Instructions: %s\n",
                                medFields[2], medFields[1], medFields[3], medFields[4], medFields[5] ? medFields[5] : "N/A");
                        medsFound = 1;
                    }
                }
                if (!medsFound) {
                    fprintf(reportFp, "    - None\n");
                }
                fclose(medFp);
            } else {
                fprintf(reportFp, "    - (Could not open medicine data file)\n");
            }
            fprintf(reportFp, "\n");
        }
    }

    if (!visitsFound) {
        fprintf(reportFp, "No emergency visit history found.\n\n");
    }
    fclose(emergencyFp);
}

void reportManagement() {
    int choice;
    initializeDataFiles(); // Ensure all data files exist


    while (1) {
        system("cls");
        printf("==== Medical Reports ====\n\n");
        printf("1. Patient Profile Report\n");
        printf("2. Patient Appointment History\n");
        printf("3. Daily Patient Report\n");
        printf("4. Patient Statistics Report\n");
        printf("5. Billing Report\n");
        printf("6. View All Reports\n");
        printf("7. Delete Report\n");
        printf("8. Back to Main Menu\n");
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
                generatePatientProfileReport();
                break;
            case 2:
                generateAppointmentHistoryReport();
                break;
            case 3:
                generateDailyPatientReport();
                break;
            case 4:
                generatePatientStatisticsReport();
                break;
            case 5:
                generateBillingReport();
                break;
            case 6:
                viewAllReports();
                break;
            case 7:
                deleteReport();
                break;
            case 8:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}