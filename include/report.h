#ifndef REPORT_H
#define REPORT_H

typedef enum {
    PATIENT_PROFILE = 1,
    APPOINTMENT_HISTORY = 2,
    DAILY_PATIENT = 3,
    PATIENT_STATISTICS = 4,
    PRESCRIPTION_REPORT = 5,
    BILLING_REPORT = 6
} ReportType;

typedef struct {
    int reportId;
    ReportType type;
    char title[100];
    char generatedDate[20];
    char generatedTime[20];
    char content[2000];
} Report;


typedef struct {
    int billId;
    int patientId;
    char patientName[50];
    char billDate[20];
    float medicineTotal;
    float consultationFee;
    float emergencyFee;
    float tax;
    float discount;
    float grandTotal;
    char paymentStatus[20];
    char paymentMethod[20];
    char notes[200];
} Bill;

// Report functions
void reportManagement();
void generatePatientProfileReport();
void generateAppointmentHistoryReport();
void generateDailyPatientReport();
void generatePatientStatisticsReport();
void generatePrescriptionReport();
void generateBillingReport();
void viewAllReports();
void deleteReport();
static void printEmergencyHistory(int patientId, FILE* reportFp);
static void printPrescriptionHistory(int patientId, FILE* reportFp);;
// Billing functions
void billingManagement();
void generatePatientBill();
void viewPatientBills();
void viewAllBills();
void updatePaymentStatus();

// Utility functions
int generateReportId();
int generatePrescriptionId();
int generateBillId();
void saveReport(Report* report);
void saveBill(Bill* bill);
Bill findBill(int billId);

#endif //REPORT_H