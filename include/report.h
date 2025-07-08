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
    int prescriptionId;
    int patientId;
    int medicineId;
    char medicineName[50];
    int quantity;
    float unitPrice;
    float totalPrice;
    char prescribedDate[20];
    char prescribedBy[50];
    char dosage[100];
    char duration[50];
    char notes[200];
} Prescription;

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
    char notes[200];  // Add this missing field
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

// Prescription functions
void prescriptionManagement();
void addPrescription();
void viewPatientPrescriptions();
void viewAllPrescriptions();
void deletePrescription();

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
void savePrescription(Prescription* prescription);
void saveBill(Bill* bill);
Prescription findPrescription(int prescriptionId);
Bill findBill(int billId);

#endif //REPORT_H