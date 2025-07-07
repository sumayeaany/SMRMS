// include/medicine.h

// Created by User on 07-Jul-25.
//

#ifndef MEDICINE_H
#define MEDICINE_H

typedef struct {
    int medicineId;
    char name[50];
    char category[30];      // Antibiotic, Painkiller, etc.
    int quantity;
    float price;
    char expiryDate[12];    // DD/MM/YYYY
    char manufacturer[50];
    char description[200];
} Medicine;

// Function declarations
void initializeMaxMedicineId();
int generateMedicineId();
void medicineInventoryLookup();
Medicine makeMedicine();
void makeMedicineEntry(Medicine* medicine);
void searchMedicine();
Medicine findMedicine(int medicineId);
void updateMedicineStock();
void listAllMedicines();
void listLowStockMedicines();
void deleteMedicine(int medicineId);


#endif //MEDICINE_H
