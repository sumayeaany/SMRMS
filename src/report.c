#include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include "report.h"
    #include "patient.h"
    #include "medicine.h"

    #define REPORT_DATAFILE "data/reports.csv"
    #define PRESCRIPTION_DATAFILE "data/prescriptions.csv"
    #define BILL_DATAFILE "data/bills.csv"

    static int maxReportId = 3000;
    static int maxPrescriptionId = 4000;
    static int maxBillId = 5000;

    // Helper functions
    static void createDataDirectory() {
        #ifdef _WIN32
            system("if not exist data mkdir data");
        #else
            system("mkdir -p data");
        #endif
    }

    int generateReportId() {
        return ++maxReportId;
    }

    int generatePrescriptionId() {
        return ++maxPrescriptionId;
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

    void savePrescription(Prescription* prescription) {
        createDataDirectory();

        FILE *fp = fopen(PRESCRIPTION_DATAFILE, "a");
        if (!fp) {
            printf("Error saving prescription.\n");
            return;
        }

        fprintf(fp, "%d,%d,%d,%s,%d,%.2f,%.2f,%s,%s,%s,%s,%s\n",
                prescription->prescriptionId, prescription->patientId, prescription->medicineId,
                prescription->medicineName, prescription->quantity, prescription->unitPrice,
                prescription->totalPrice, prescription->prescribedDate, prescription->prescribedBy,
                prescription->dosage, prescription->duration, prescription->notes);

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

    void generatePatientProfileReport() {
        int patientId;
        printf("Enter Patient ID: ");
        scanf("%d", &patientId);
        getchar();

        char patientIdStr[10];
        sprintf(patientIdStr, "%d", patientId);
        Patient patient = findPatient(3, patientIdStr);

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

        char content[2000];
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
                "Primary Doctor: %s\n\n"
                "Report generated on: %s\n",
                patient.patientId, patient.name, patient.age,
                patient.gender == 'M' ? "Male" : "Female",
                patient.phone, patient.address, patient.email,
                patient.bloodType, patient.allergies,
                patient.emergencyContact, patient.primaryDoctor,
                report.generatedDate);

        strcpy(report.content, content);
        saveReport(&report);

        printf("\n%s", content);
        printf("\nReport saved with ID: %d\n", report.reportId);
        printf("Press Enter to return to menu...");
        getchar();
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

    void generatePrescriptionReport() {
        int patientId;
        printf("Enter Patient ID (0 for all patients): ");
        scanf("%d", &patientId);
        getchar();

        Report report = {0};
        report.reportId = generateReportId();
        report.type = PRESCRIPTION_REPORT;

        if (patientId == 0) {
            strcpy(report.title, "All Prescriptions Report");
        } else {
            sprintf(report.title, "Prescriptions Report - Patient ID: %d", patientId);
        }

        char content[2000] = "==== PRESCRIPTION REPORT ====\n\n";
        char line[200];

        FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
        if (!fp) {
            strcat(content, "No prescription data found.\n");
        } else {
            int prescriptionCount = 0;
            float totalAmount = 0.0;

            Prescription prescription;
            while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                          &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                          prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                          &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                          prescription.dosage, prescription.duration, prescription.notes) == 12) {

                if (patientId == 0 || prescription.patientId == patientId) {
                    sprintf(line, "Prescription ID: %d\n", prescription.prescriptionId);
                    strcat(content, line);
                    sprintf(line, "Patient ID: %d\n", prescription.patientId);
                    strcat(content, line);
                    sprintf(line, "Medicine: %s\n", prescription.medicineName);
                    strcat(content, line);
                    sprintf(line, "Quantity: %d\n", prescription.quantity);
                    strcat(content, line);
                    sprintf(line, "Unit Price: $%.2f\n", prescription.unitPrice);
                    strcat(content, line);
                    sprintf(line, "Total: $%.2f\n", prescription.totalPrice);
                    strcat(content, line);
                    sprintf(line, "Date: %s\n", prescription.prescribedDate);
                    strcat(content, line);
                    sprintf(line, "Prescribed By: %s\n", prescription.prescribedBy);
                    strcat(content, line);
                    sprintf(line, "Dosage: %s\n", prescription.dosage);
                    strcat(content, line);
                    sprintf(line, "Duration: %s\n\n", prescription.duration);
                    strcat(content, line);

                    prescriptionCount++;
                    totalAmount += prescription.totalPrice;
                }
            }
            fclose(fp);

            sprintf(line, "Total Prescriptions: %d\n", prescriptionCount);
            strcat(content, line);
            sprintf(line, "Total Amount: $%.2f\n", totalAmount);
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

        char content[2000] = "==== BILLING REPORT ====\n\n";
        char line[200];

        FILE *fp = fopen(BILL_DATAFILE, "r");
        if (!fp) {
            strcat(content, "No billing data found.\n");
        } else {
            int billCount = 0;
            float totalRevenue = 0.0;

            Bill bill;
            while (fscanf(fp, "%d,%d,%f,%f,%f,%f,%f,%19[^,],%19[^,],%199[^\n]",
                          &bill.billId, &bill.patientId, &bill.consultationFee, &bill.medicineTotal,
                          &bill.tax, &bill.discount, &bill.grandTotal, bill.billDate,
                          bill.paymentStatus, bill.notes) == 10) {

                if (patientId == 0 || bill.patientId == patientId) {
                    sprintf(line, "Bill ID: %d\n", bill.billId);
                    strcat(content, line);
                    sprintf(line, "Patient ID: %d\n", bill.patientId);
                    strcat(content, line);
                    sprintf(line, "Consultation Fee: $%.2f\n", bill.consultationFee);
                    strcat(content, line);
                    sprintf(line, "Medicine Total: $%.2f\n", bill.medicineTotal);
                    strcat(content, line);
                    sprintf(line, "Tax: $%.2f\n", bill.tax);
                    strcat(content, line);
                    sprintf(line, "Discount: $%.2f\n", bill.discount);
                    strcat(content, line);
                    sprintf(line, "Grand Total: $%.2f\n", bill.grandTotal);
                    strcat(content, line);
                    sprintf(line, "Date: %s\n", bill.billDate);
                    strcat(content, line);
                    sprintf(line, "Payment Status: %s\n\n", bill.paymentStatus);
                    strcat(content, line);

                    billCount++;
                    totalRevenue += bill.grandTotal;
                }
            }
            fclose(fp);

            sprintf(line, "Total Bills: %d\n", billCount);
            strcat(content, line);
            sprintf(line, "Total Revenue: $%.2f\n", totalRevenue);
            strcat(content, line);
        }

        strcpy(report.content, content);
        saveReport(&report);

        printf("\n%s", content);
        printf("\nReport saved with ID: %d\n", report.reportId);
        printf("Press Enter to return to menu...");
        getchar();
    }

    void prescriptionManagement() {
        int choice;

        while (1) {
            system("cls");
            printf("==== Prescription Management ====\n\n");
            printf("1. Add Prescription\n");
            printf("2. View Patient Prescriptions\n");
            printf("3. View All Prescriptions\n");
            printf("4. Delete Prescription\n");
            printf("5. Back to Reports Menu\n");
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
                    addPrescription();
                    break;
                case 2:
                    viewPatientPrescriptions();
                    break;
                case 3:
                    viewAllPrescriptions();
                    break;
                case 4: {
                    int prescriptionId;
                    printf("Enter Prescription ID to delete: ");
                    scanf("%d", &prescriptionId);
                    getchar();
                    deletePrescription(prescriptionId);
                    break;
                }
                case 5:
                    return;
                default:
                    printf("Invalid choice. Press Enter to continue...");
                    getchar();
            }
        }
    }

    void addPrescription() {
        Prescription prescription = {0};
        prescription.prescriptionId = generatePrescriptionId();

        printf("\n==== Add New Prescription ====\n");

        printf("Patient ID: ");
        scanf("%d", &prescription.patientId);
        getchar();

        printf("Medicine ID: ");
        scanf("%d", &prescription.medicineId);
        getchar();

        // Get medicine details
        const Medicine medicine = findMedicine(prescription.medicineId);
        if (medicine.medicineId == 0) {
            printf("Medicine not found!\n");
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }

        strcpy(prescription.medicineName, medicine.name);
        prescription.unitPrice = medicine.price;

        printf("Quantity: ");
        scanf("%d", &prescription.quantity);
        getchar();

        prescription.totalPrice = prescription.quantity * prescription.unitPrice;

        printf("Prescribed by (Doctor name): ");
        fgets(prescription.prescribedBy, sizeof(prescription.prescribedBy), stdin);
        prescription.prescribedBy[strcspn(prescription.prescribedBy, "\n")] = 0;

        printf("Dosage: ");
        fgets(prescription.dosage, sizeof(prescription.dosage), stdin);
        prescription.dosage[strcspn(prescription.dosage, "\n")] = 0;

        printf("Duration: ");
        fgets(prescription.duration, sizeof(prescription.duration), stdin);
        prescription.duration[strcspn(prescription.duration, "\n")] = 0;

        printf("Notes: ");
        fgets(prescription.notes, sizeof(prescription.notes), stdin);
        prescription.notes[strcspn(prescription.notes, "\n")] = 0;

        // Set current date
        time_t now;
        time(&now);
        const struct tm *timeinfo = localtime(&now);
        strftime(prescription.prescribedDate, sizeof(prescription.prescribedDate), "%d/%m/%Y", timeinfo);

        savePrescription(&prescription);

        printf("Prescription added successfully with ID: %d\n", prescription.prescriptionId);
        printf("Total cost: $%.2f\n", prescription.totalPrice);
        printf("Press Enter to return to menu...");
        getchar();
    }

    void viewPatientPrescriptions() {
        int patientId;
        printf("Enter Patient ID: ");
        scanf("%d", &patientId);
        getchar();

        FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
        if (!fp) {
            printf("No prescription data found.\n");
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }

        printf("\n==== Prescriptions for Patient ID: %d ====\n", patientId);
        printf("%-5s %-20s %-4s %-10s %-10s %-12s %-15s\n",
               "ID", "Medicine", "Qty", "Unit Price", "Total", "Date", "Prescribed By");
        printf("--------------------------------------------------------------------------------\n");

        Prescription prescription;
        int found = 0;

        while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                      &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                      prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                      &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                      prescription.dosage, prescription.duration, prescription.notes) == 12) {

            if (prescription.patientId == patientId) {
                printf("%-5d %-20s %-4d $%-9.2f $%-9.2f %-12s %-15s\n",
                       prescription.prescriptionId, prescription.medicineName, prescription.quantity,
                       prescription.unitPrice, prescription.totalPrice, prescription.prescribedDate,
                       prescription.prescribedBy);
                found = 1;
            }
        }

        if (!found) {
            printf("No prescriptions found for this patient.\n");
        }

        fclose(fp);
        printf("Press Enter to return to menu...");
        getchar();
    }

    void viewAllPrescriptions() {
        FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
        if (!fp) {
            printf("No prescription data found.\n");
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }

        printf("\n==== All Prescriptions ====\n");
        printf("%-5s %-10s %-20s %-4s %-10s %-10s %-12s\n",
               "ID", "Patient", "Medicine", "Qty", "Unit Price", "Total", "Date");
        printf("--------------------------------------------------------------------------------\n");

        Prescription prescription;
        while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                      &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                      prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                      &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                      prescription.dosage, prescription.duration, prescription.notes) == 12) {

            printf("%-5d %-10d %-20s %-4d $%-9.2f $%-9.2f %-12s\n",
                   prescription.prescriptionId, prescription.patientId, prescription.medicineName,
                   prescription.quantity, prescription.unitPrice, prescription.totalPrice,
                   prescription.prescribedDate);
        }

        fclose(fp);
        printf("Press Enter to return to menu...");
        getchar();
    }

    void deletePrescription(const int prescriptionId) {
        FILE *fp = fopen(PRESCRIPTION_DATAFILE, "r");
        if (!fp) {
            printf("No prescription data found.\n");
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }

        FILE *temp = fopen("data/temp_prescriptions.csv", "w");
        if (!temp) {
            printf("Error creating temporary file.\n");
            fclose(fp);
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }

        Prescription prescription;
        int found = 0;

        while (fscanf(fp, "%d,%d,%d,%49[^,],%d,%f,%f,%19[^,],%49[^,],%99[^,],%49[^,],%199[^\n]",
                      &prescription.prescriptionId, &prescription.patientId, &prescription.medicineId,
                      prescription.medicineName, &prescription.quantity, &prescription.unitPrice,
                      &prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                      prescription.dosage, prescription.duration, prescription.notes) == 12) {

            if (prescription.prescriptionId != prescriptionId) {
                fprintf(temp, "%d,%d,%d,%s,%d,%.2f,%.2f,%s,%s,%s,%s,%s\n",
                        prescription.prescriptionId, prescription.patientId, prescription.medicineId,
                        prescription.medicineName, prescription.quantity, prescription.unitPrice,
                        prescription.totalPrice, prescription.prescribedDate, prescription.prescribedBy,
                        prescription.dosage, prescription.duration, prescription.notes);
            } else {
                found = 1;
            }
        }

        fclose(fp);
        fclose(temp);

        if (found) {
            remove(PRESCRIPTION_DATAFILE);
            rename("data/temp_prescriptions.csv", PRESCRIPTION_DATAFILE);
            printf("Prescription deleted successfully.\n");
        } else {
            remove("data/temp_prescriptions.csv");
            printf("Prescription with ID %d not found.\n", prescriptionId);
        }

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

    void reportManagement() {
        int choice;

        while (1) {
            system("cls");
            printf("==== Medical Reports ====\n\n");
            printf("1. Patient Profile Report\n");
            printf("2. Patient Appointment History\n");
            printf("3. Daily Patient Report\n");
            printf("4. Patient Statistics Report\n");
            printf("5. Prescription Report\n");
            printf("6. Billing Report\n");
            printf("7. Prescription Management\n");
            printf("8. View All Reports\n");
            printf("9. Delete Report\n");
            printf("10. Back to Main Menu\n");
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
                    generatePrescriptionReport();
                    break;
                case 6:
                    generateBillingReport();
                    break;
                case 7:
                    prescriptionManagement();
                    break;
                case 8:
                    viewAllReports();
                    break;
                case 9:
                    deleteReport();
                    break;
                case 10:
                    return;
                default:
                    printf("Invalid choice. Press Enter to continue...");
                    getchar();
            }
        }
    }