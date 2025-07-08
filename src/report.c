#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "report.h"
#include "patient.h"
#include "prescription.h"

#define REPORT_DATAFILE "data/reports.csv"
#define PRESCRIPTION_DATAFILE "data/prescription.csv"
#define BILL_DATAFILE "data/bills.csv"
#define EMERGENCY_DATAFILE "data/emergency.csv"

static int maxReportId = 3000;
static int maxBillId = 5000;

// Helper functions
static void createDataDirectory() {
    #ifdef _WIN32
        system("if not exist data mkdir data");
    #else

    #endif
}

int generateReportId() {
    return ++maxReportId;
}


int generateBillId() {
    return ++maxBillId;
}

void saveReport(Report* report) {
    createDataDirectory();

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
    createDataDirectory();

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
    printf("Enter Patient ID (0 for all patients): ");
    scanf("%d", &patientId);
    getchar();

    Report report = {0};
    report.reportId = generateReportId();
    report.type = BILLING_REPORT;

    if (patientId == 0) {
        strcpy(report.title, "All Bills Report");
    } else {
        sprintf(report.title, "Billing Report - Patient ID: %d", patientId);
    }

    char content[4000] = "==== BILLING REPORT ====\n\n";
    char line[300];

    // Try to read from bills file first
    FILE *fp = fopen(BILL_DATAFILE, "r");

    if (fp) {
        float totalRevenue = 0.0;
        int billCount = 0;
        Bill bill;
        while (fscanf(fp, "%d,%d,%f,%f,%f,%f,%f,%19[^,],%19[^,],%199[^\n]",
                      &bill.billId, &bill.patientId, &bill.consultationFee, &bill.medicineTotal,
                      &bill.tax, &bill.discount, &bill.grandTotal, bill.billDate,
                      bill.paymentStatus, bill.notes) == 10) {

            if (patientId == 0 || bill.patientId == patientId) {
                sprintf(line, "Bill ID: %d | Patient ID: %d | Total: Tk.%.2f | Date: %s | Status: %s\n",
                        bill.billId, bill.patientId, bill.grandTotal, bill.billDate, bill.paymentStatus);
                strcat(content, line);
                billCount++;
                totalRevenue += bill.grandTotal;
            }
        }
        fclose(fp);

        if (billCount > 0) {
            sprintf(line, "\nTotal Bills: %d\nTotal Revenue: Tk.%.2f\n\n", billCount, totalRevenue);
            strcat(content, line);
        }
    }

    // Generate from prescription data
    strcat(content, "==== PRESCRIPTION COSTS ====\n");
    FILE *prescFp = fopen("data/prescription.csv", "r");
    float totalPrescriptionCost = 0.0;
    int prescriptionCount = 0;

    if (prescFp) {
        Prescription prescription;
        while (fscanf(prescFp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                      &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                      prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                      &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                      prescription.dosage, prescription.duration, prescription.notes) == 12) {

            if (patientId == 0 || prescription.patientId == patientId) {
                sprintf(line, "Prescription ID: %d | Patient ID: %d | Medicine: %s | Cost: Tk.%.2f | Date: %s\n",
                        prescription.prescriptionId, prescription.patientId, prescription.medicineName,
                        prescription.totalPrice, prescription.prescribedDate);
                strcat(content, line);
                totalPrescriptionCost += prescription.totalPrice;
                prescriptionCount++;
            }
        }
        fclose(prescFp);
    }

    // Add emergency medicine costs for discharged patients
    strcat(content, "\n==== EMERGENCY MEDICINE COSTS ====\n");
    FILE *emergencyFp = fopen("data/emergency_records.csv", "r");
    float totalEmergencyCost = 0.0;
    int emergencyCount = 0;

    if (emergencyFp) {
        char emergencyLine[500];
        while (fgets(emergencyLine, sizeof(emergencyLine), emergencyFp)) {
            int emergencyId, emergPatientId, priority;
            char patientName[50], phoneNumber[20], symptoms[200], arrivalDate[20], arrivalTime[10];
            char status[20], assignedDoctor[50], treatment[200], notes[200];

            if (sscanf(emergencyLine, "%d,%d,%49[^,],%19[^,],%199[^,],%d,%19[^,],%9[^,],%19[^,],%49[^,],%199[^,],%199[^\n]",
                      &emergencyId, &emergPatientId, patientName, phoneNumber, symptoms, &priority,
                      arrivalDate, arrivalTime, status, assignedDoctor, treatment, notes) == 12) {

                if ((patientId == 0 || emergPatientId == patientId) &&
                    (strcmp(status, "Discharged") == 0 || strcmp(status, "Completed") == 0)) {

                    // Calculate emergency treatment cost (base emergency fee + treatment cost)
                    const float emergencyFee = 1000.0; // Base emergency treatment fee
                    float treatmentCost = 0.0;

                    // Parse treatment for medicine costs
                    if (strstr(treatment, "Medicine") != NULL || strstr(treatment, "medication") != NULL ||
                        strstr(treatment, "Paracetamol") != NULL || strstr(treatment, "Antibiotic") != NULL ||
                        strstr(treatment, "injection") != NULL || strstr(treatment, "IV") != NULL) {
                        treatmentCost = 500.0; // Default medicine cost for emergency treatment
                    }

                    const float totalEmergencyItemCost = emergencyFee + treatmentCost;

                    sprintf(line, "Emergency ID: %d | Patient ID: %d | Treatment: %s | Cost: Tk.%.2f | Date: %s\n",
                            emergencyId, emergPatientId, treatment, totalEmergencyItemCost, arrivalDate);
                    strcat(content, line);

                    // This is the key fix - properly accumulate the cost
                    totalEmergencyCost += totalEmergencyItemCost;
                    emergencyCount++;
                }
            }
        }
        fclose(emergencyFp);

        if (emergencyCount == 0) {
            strcat(content, "No emergency medicine costs found.\n");
        } else {
            sprintf(line, "\nTotal Emergency Records: %d\n", emergencyCount);
            strcat(content, line);
            sprintf(line, "Total Emergency Cost: Tk.%.2f\n", totalEmergencyCost);
            strcat(content, line);
        }
    } else {
        strcat(content, "No emergency data available.\n");
    }

    // Calculate total costs
    const int totalServices = prescriptionCount + emergencyCount;
    if (totalServices > 0) {
        sprintf(line, "\nTotal Prescriptions: %d\nTotal Emergency Treatments: %d\n",
                prescriptionCount, emergencyCount);
        strcat(content, line);
        sprintf(line, "Total Prescription Cost: Tk.%.2f\nTotal Emergency Cost: Tk.%.2f\n",
                totalPrescriptionCost, totalEmergencyCost);
        strcat(content, line);

        // Calculate estimated billing with consultation fees and tax
        const float consultationFees = prescriptionCount * 500.0; // 500 Tk per prescription
        const float emergencyFees = emergencyCount * 200.0; // Additional 200 Tk per emergency consultation
        const float subtotal = totalPrescriptionCost + totalEmergencyCost + consultationFees + emergencyFees;
        const float tax = subtotal * 0.05; // 5% tax
        const float grandTotal = subtotal + tax;

        sprintf(line, "\n==== ESTIMATED BILLING SUMMARY ====\n");
        strcat(content, line);
        sprintf(line, "Prescription Medicine Costs: Tk.%.2f\n", totalPrescriptionCost);
        strcat(content, line);
        sprintf(line, "Emergency Medicine Costs: Tk.%.2f\n", totalEmergencyCost);
        strcat(content, line);
        sprintf(line, "Consultation Fees: Tk.%.2f\n", consultationFees);
        strcat(content, line);
        sprintf(line, "Emergency Consultation Fees: Tk.%.2f\n", emergencyFees);
        strcat(content, line);
        sprintf(line, "Tax (5%%): Tk.%.2f\n", tax);
        strcat(content, line);
        sprintf(line, "Estimated Grand Total: Tk.%.2f\n", grandTotal);
        strcat(content, line);
    } else {
        strcat(content, "No prescription or emergency data found for billing.\n");
    }

    strcpy(report.content, content);
    saveReport(&report);

    printf("\n%s", content);
    printf("\nReport saved with ID: %d\n", report.reportId);
    printf("Press Enter to return to menu...");
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
void generatePatientProfileReport() {
    int patientId;
    printf("Enter Patient ID: ");
    scanf("%d", &patientId);
    getchar();

    char patientIdStr[10];
    sprintf(patientIdStr, "%d", patientId);
    Patient patient = findPatientBySearch(3, patientIdStr, NULL);

    if (patient.patientId == 0) {
        printf("Patient not found!\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Report report = {0};
    report.reportId = generateReportId();
    report.type = PATIENT_PROFILE;
    sprintf(report.title, "Patient Profile - %s (ID: %d)", patient.name, patient.patientId);

    char content[4000]; // Increased size for prescription data
    sprintf(content,
            "==== PATIENT PROFILE REPORT ====\n\n"
            "Patient ID: %d\n"
            "Name: %s\n"
            "Age: %d\n"
            "Gender: %s\n"
            "Phone: %s\n"
            "Address: %s\n"
            "Email: %s\n"
            "Blood Type: %s\n"
            "Allergies: %s\n"
            "Emergency Contact: %s\n"
            "Primary Doctor: %s\n\n",
            patient.patientId, patient.name, patient.age,
            patient.gender == 'M' ? "Male" : "Female",
            patient.phone, patient.address, patient.email,
            patient.bloodType, patient.allergies,
            patient.emergencyContact, patient.primaryDoctor);

    // Add prescription information
    strcat(content, "==== PRESCRIPTION HISTORY ====\n");

    FILE *prescriptionFp = fopen(PRESCRIPTION_DATAFILE, "r");
    if (prescriptionFp) {
        char line[500];
        int prescriptionCount = 0;

        while (fgets(line, sizeof(line), prescriptionFp)) {
            int prescId, prescPatientId, medicineId, quantity;
            float unitPrice, totalPrice;
            char medicineName[50], prescribedDate[20], prescribedBy[50], dosage[100], duration[50], notes[200];

            if (sscanf(line, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                      &prescId, &prescPatientId, &medicineId, medicineName, &quantity, &unitPrice,
                      &totalPrice, prescribedDate, prescribedBy, dosage, duration, notes) == 12) {

                if (prescPatientId == patientId) {
                    char prescLine[400];
                    sprintf(prescLine, "Prescription ID: %d\n", prescId);
                    strcat(content, prescLine);
                    sprintf(prescLine, "Date: %s | Doctor: %s\n", prescribedDate, prescribedBy);
                    strcat(content, prescLine);
                    sprintf(prescLine, "Medicine: %s | Quantity: %d | Unit Price: Tk.%.2f\n",
                            medicineName, quantity, unitPrice);
                    strcat(content, prescLine);
                    sprintf(prescLine, "Total: Tk.%.2f | Dosage: %s\n", totalPrice, dosage);
                    strcat(content, prescLine);
                    sprintf(prescLine, "Duration: %s | Notes: %s\n\n", duration, notes);
                    strcat(content, prescLine);
                    prescriptionCount++;
                }
            }
        }
        fclose(prescriptionFp);

        if (prescriptionCount == 0) {
            strcat(content, "No prescription history found.\n\n");
        }
    } else {
        strcat(content, "No prescription data available.\n\n");
    }

    // Add emergency medicine history
    strcat(content, "==== EMERGENCY MEDICINE HISTORY ====\n");

    FILE *emergencyFp = fopen(EMERGENCY_DATAFILE, "r");
    if (emergencyFp) {
        char line[500];
        int emergencyCount = 0;

        while (fgets(line, sizeof(line), emergencyFp)) {
            int emergencyId, emergPatientId, priority;
            char patientName[50], symptoms[200], arrivalDate[20], arrivalTime[10];
            char status[20], assignedDoctor[50], treatment[200], notes[200];

            // Parse the complete emergency record format
            if (sscanf(line, "%d,%d,%49[^,],%*[^,],%199[^,],%d,%19[^,],%9[^,],%19[^,],%49[^,],%199[^,],%199[^\n]",
                      &emergencyId, &emergPatientId, patientName, symptoms, &priority,
                      arrivalDate, arrivalTime, status, assignedDoctor, treatment, notes) >= 10) {

                if (emergPatientId == patientId) {
                    char emergLine[400];
                    sprintf(emergLine, "Emergency ID: %d\n", emergencyId);
                    strcat(content, emergLine);
                    sprintf(emergLine, "Date: %s | Time: %s\n", arrivalDate, arrivalTime);
                    strcat(content, emergLine);
                    sprintf(emergLine, "Priority: %d | Status: %s\n", priority, status);
                    strcat(content, emergLine);
                    sprintf(emergLine, "Symptoms: %s\n", symptoms);
                    strcat(content, emergLine);
                    sprintf(emergLine, "Doctor: %s\n", assignedDoctor);
                    strcat(content, emergLine);
                    sprintf(emergLine, "Treatment: %s\n", treatment);
                    strcat(content, emergLine);
                    sprintf(emergLine, "Notes: %s\n\n", notes);
                    strcat(content, emergLine);
                    emergencyCount++;
                }
            }
        }
        fclose(emergencyFp);

        if (emergencyCount == 0) {
            strcat(content, "No emergency treatment history found.\n\n");
        } else {
            char countLine[50];
            sprintf(countLine, "Total Emergency Records: %d\n\n", emergencyCount);
            strcat(content, countLine);
        }
    } else {
        strcat(content, "No emergency data available.\n\n");
    }

    char reportDate[20];
    time_t now;
    time(&now);
    const struct tm *timeinfo = localtime(&now);
    strftime(reportDate, sizeof(reportDate), "%d/%m/%Y", timeinfo);

    char footerLine[100];
    sprintf(footerLine, "Report generated on: %s\n", reportDate);
    strcat(content, footerLine);

    strcpy(report.content, content);
    strcpy(report.generatedDate, reportDate);
    saveReport(&report);

    printf("\n%s", content);
    printf("\nReport saved with ID: %d\n", report.reportId);
    printf("Press Enter to return to menu...");
    getchar();
}

void reportManagement() {
    int choice;

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