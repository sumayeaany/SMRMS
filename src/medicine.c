#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "medicine.h"

#define MEDICINE_DATAFILE "data/medicine.csv"

static int maxMedicineId = 0;
static int isMaxMedicineIdInitialized = 0;
// Helper function to check if a string is effectively empty
static int isMedicineEffectivelyEmpty(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// Helper function for input
static void getMedicineInput(const char* prompt, char* dest, size_t size) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)size, stdin)) {
        dest[strcspn(dest, "\n")] = 0;
    } else {
        dest[0] = '\0';
    }
}

// Helper: set "N/A" if input is empty
static void setMedicineOrNA(char* dest, const char* input, const size_t size) {
    if (!isMedicineEffectivelyEmpty(input))
        strncpy(dest, input, size - 1);
    else
        strncpy(dest, "N/A", size - 1);
    dest[size - 1] = '\0';
}

void initializeMaxMedicineId() {
    if (isMaxMedicineIdInitialized) return;

    FILE *fp = fopen(MEDICINE_DATAFILE, "r");
    if (!fp) {
        maxMedicineId = 0;
        isMaxMedicineIdInitialized = 1;
        return;
    }

    Medicine medicine;
    maxMedicineId = 0;
    while (fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%11[^,],%49[^,],%199[^\n]",
                  &medicine.medicineId, medicine.name, medicine.category,
                  &medicine.quantity, &medicine.price, medicine.expiryDate,
                  medicine.manufacturer, medicine.description) == 8) {
        if (medicine.medicineId > maxMedicineId) {
            maxMedicineId = medicine.medicineId;
        }
    }
    fclose(fp);
    isMaxMedicineIdInitialized = 1;
}

int generateMedicineId() {
    if (!isMaxMedicineIdInitialized) {
        initializeMaxMedicineId();
    }
    return ++maxMedicineId;
}

void stripMedicineNewline(char* str) {
    const size_t len = strlen(str);
    if (len > 0 && str[len-1] == '\n') {
        str[len-1] = '\0';
    }
}

void makeMedicineEntry(Medicine* medicine) {
    // Create data directory if it doesn't exist
    #ifdef _WIN32
        system("if not exist data mkdir data");
    #else
        system("mkdir -p data");
    #endif

    FILE *fp = fopen(MEDICINE_DATAFILE, "a");
    if (!fp) {
        fp = fopen(MEDICINE_DATAFILE, "w");
        if (!fp) {
            perror("Unable to create medicine data file");
            printf("Press Enter to return to menu...");
            getchar();
            return;
        }
        fclose(fp);
        fp = fopen(MEDICINE_DATAFILE, "a");
    }

    if (medicine->medicineId == 0) {
        medicine->medicineId = generateMedicineId();
    }

    fprintf(fp, "%d,%s,%s,%d,%.2f,%s,%s,%s\n",
            medicine->medicineId, medicine->name, medicine->category,
            medicine->quantity, medicine->price, medicine->expiryDate,
            medicine->manufacturer, medicine->description);

    fclose(fp);
    printf("Medicine added successfully with ID: %d\n", medicine->medicineId);
    printf("Press Enter to return to menu...");
    getchar();
}

Medicine makeMedicine() {
    Medicine medicine = {0};
    char buffer[256];

    printf("\n==== Add New Medicine ====\n");

    // Medicine name
    getMedicineInput("Medicine name: ", buffer, sizeof(buffer));
    setMedicineOrNA(medicine.name, buffer, sizeof(medicine.name));

    // Category
    getMedicineInput("Category: ", buffer, sizeof(buffer));
    setMedicineOrNA(medicine.category, buffer, sizeof(medicine.category));

    // Quantity
    getMedicineInput("Quantity: ", buffer, sizeof(buffer));
    medicine.quantity = atoi(buffer);

    // Price
    getMedicineInput("Price: ", buffer, sizeof(buffer));
    medicine.price = (float)atof(buffer);

    // Expiry date
    getMedicineInput("Expiry date (DD/MM/YYYY): ", buffer, sizeof(buffer));
    setMedicineOrNA(medicine.expiryDate, buffer, sizeof(medicine.expiryDate));

    // Manufacturer
    getMedicineInput("Manufacturer: ", buffer, sizeof(buffer));
    setMedicineOrNA(medicine.manufacturer, buffer, sizeof(medicine.manufacturer));

    // Description
    getMedicineInput("Description: ", buffer, sizeof(buffer));
    setMedicineOrNA(medicine.description, buffer, sizeof(medicine.description));

    return medicine;
}

Medicine findMedicine(const int medicineId) {
    Medicine medicine = {0};
    FILE *fp = fopen(MEDICINE_DATAFILE, "r");
    if (!fp) {
        printf("Unable to open medicine data file\n");
        return medicine;
    }

    while (fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%11[^,],%49[^,],%199[^\n]",
                  &medicine.medicineId, medicine.name, medicine.category,
                  &medicine.quantity, &medicine.price, medicine.expiryDate,
                  medicine.manufacturer, medicine.description) == 8) {
        if (medicine.medicineId == medicineId) {
            fclose(fp);
            return medicine;
        }
    }
    fclose(fp);
    medicine.medicineId = 0; // Not found
    return medicine;
}

void showMedicine(Medicine* medicine) {
    printf("\n==== Medicine Details ====\n");
    printf("Medicine ID: %d\n", medicine->medicineId);
    printf("Name: %s\n", medicine->name);
    printf("Category: %s\n", medicine->category);
    printf("Quantity: %d\n", medicine->quantity);
    printf("Price: $%.2f\n", medicine->price);
    printf("Expiry Date: %s\n", medicine->expiryDate);
    printf("Manufacturer: %s\n", medicine->manufacturer);
    printf("Description: %s\n", medicine->description);
    printf("============================\n");
}

void listAllMedicines() {
    FILE *fp = fopen(MEDICINE_DATAFILE, "r");
    if (!fp) {
        printf("No medicine data file found or unable to open file.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Medicine medicine;
    printf("\n==== All Medicines ====\n");
    printf("%-5s %-20s %-15s %-8s %-8s %-12s\n", "ID", "Name", "Category", "Quantity", "Price", "Expiry");
    printf("----------------------------------------------------------------\n");

    while (fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%11[^,],%49[^,],%199[^\n]",
                  &medicine.medicineId, medicine.name, medicine.category,
                  &medicine.quantity, &medicine.price, medicine.expiryDate,
                  medicine.manufacturer, medicine.description) == 8) {
        printf("%-5d %-20s %-15s %-8d Tk.%-7.2f %-12s\n",
               medicine.medicineId, medicine.name, medicine.category,
               medicine.quantity, medicine.price, medicine.expiryDate);
    }
    fclose(fp);
    printf("\nPress Enter to return to menu...");
    getchar();
}

void listLowStockMedicines() {
    int threshold;
    printf("Enter minimum stock threshold: ");
    scanf("%d", &threshold);
    getchar(); // consume newline

    FILE *fp = fopen(MEDICINE_DATAFILE, "r");
    if (!fp) {
        printf("No medicine data file found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Medicine medicine;
    printf("\n==== Low Stock Medicines (< %d) ====\n", threshold);
    printf("%-5s %-20s %-8s\n", "ID", "Name", "Quantity");
    printf("--------------------------------\n");

    int found = 0;
    while (fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%11[^,],%49[^,],%199[^\n]",
                  &medicine.medicineId, medicine.name, medicine.category,
                  &medicine.quantity, &medicine.price, medicine.expiryDate,
                  medicine.manufacturer, medicine.description) == 8) {
        if (medicine.quantity < threshold) {
            printf("%-5d %-20s %-8d\n", medicine.medicineId, medicine.name, medicine.quantity);
            found = 1;
        }
    }

    if (!found) {
        printf("No medicines found with low stock.\n");
    }

    fclose(fp);
    printf("\nPress Enter to return to menu...");
    getchar();
}

void updateMedicineStock() {
    int medicineId, newQuantity;
    printf("Enter Medicine ID to update stock: ");
    scanf("%d", &medicineId);
    getchar();

    Medicine medicine = findMedicine(medicineId);
    if (medicine.medicineId == 0) {
        printf("Medicine with ID %d not found.\n", medicineId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    showMedicine(&medicine);
    printf("Current quantity: %d\n", medicine.quantity);
    printf("Enter new quantity: ");
    scanf("%d", &newQuantity);
    getchar();

    // Update the file by rewriting it
    FILE *fp = fopen(MEDICINE_DATAFILE, "r");
    FILE *temp = fopen("data/temp_medicine.csv", "w");

    if (!fp || !temp) {
        printf("Error accessing files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Medicine currentMedicine;
    while (fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%11[^,],%49[^,],%199[^\n]",
                  &currentMedicine.medicineId, currentMedicine.name, currentMedicine.category,
                  &currentMedicine.quantity, &currentMedicine.price, currentMedicine.expiryDate,
                  &currentMedicine.manufacturer, currentMedicine.description) == 8) {

        if (currentMedicine.medicineId == medicineId) {
            // Write updated record
            fprintf(temp, "%d,%s,%s,%d,%.2f,%s,%s,%s\n",
                    currentMedicine.medicineId, currentMedicine.name, currentMedicine.category,
                    newQuantity, currentMedicine.price, currentMedicine.expiryDate,
                    currentMedicine.manufacturer, currentMedicine.description);
        } else {
            // Write original record
            fprintf(temp, "%d,%s,%s,%d,%.2f,%s,%s,%s\n",
                    currentMedicine.medicineId, currentMedicine.name, currentMedicine.category,
                    currentMedicine.quantity, currentMedicine.price, currentMedicine.expiryDate,
                    currentMedicine.manufacturer, currentMedicine.description);
        }
    }

    fclose(fp);
    fclose(temp);

    // Replace original file with updated file
    remove(MEDICINE_DATAFILE);
    rename("data/temp_medicine.csv", MEDICINE_DATAFILE);

    printf("Stock updated successfully from %d to %d.\n", medicine.quantity, newQuantity);
    printf("Press Enter to return to menu...");
    getchar();
}

void deleteMedicine(const int medicineId) {
    Medicine medicine = findMedicine(medicineId);
    if (medicine.medicineId == 0) {
        printf("Medicine with ID %d not found.\n", medicineId);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    showMedicine(&medicine);
    printf("\nAre you sure you want to delete this medicine? (y/n): ");
    char confirm;
    scanf("%c", &confirm);
    getchar();

    if (confirm != 'y' && confirm != 'Y') {
        printf("Delete operation cancelled.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    FILE *fp = fopen(MEDICINE_DATAFILE, "r");
    FILE *temp = fopen("data/temp_medicine.csv", "w");

    if (!fp || !temp) {
        printf("Error accessing files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Medicine currentMedicine;
    while (fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%11[^,],%49[^,],%199[^\n]",
                  &currentMedicine.medicineId, currentMedicine.name, currentMedicine.category,
                  &currentMedicine.quantity, &currentMedicine.price, currentMedicine.expiryDate,
                  currentMedicine.manufacturer, currentMedicine.description) == 8) {

        // Only write records that don't match the ID to delete
        if (currentMedicine.medicineId != medicineId) {
            fprintf(temp, "%d,%s,%s,%d,%.2f,%s,%s,%s\n",
                    currentMedicine.medicineId, currentMedicine.name, currentMedicine.category,
                    currentMedicine.quantity, currentMedicine.price, currentMedicine.expiryDate,
                    currentMedicine.manufacturer, currentMedicine.description);
        }
    }

    fclose(fp);
    fclose(temp);

    // Replace original file with updated file
    remove(MEDICINE_DATAFILE);
    rename("data/temp_medicine.csv", MEDICINE_DATAFILE);

    printf("Medicine deleted successfully.\n");
    printf("Press Enter to return to menu...");
    getchar();
}

void searchMedicine() {
    char searchName[50];
    printf("Enter medicine name to search: ");
    fgets(searchName, sizeof(searchName), stdin);
    stripMedicineNewline(searchName);

    FILE *fp = fopen(MEDICINE_DATAFILE, "r");
    if (!fp) {
        printf("No medicine data file found.\n");
        printf("Press Enter to return to menu...");
        getchar();
        return;
    }

    Medicine medicine;
    int found = 0;
    printf("\n==== Search Results ====\n");

    while (fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%11[^,],%49[^,],%199[^\n]",
                  &medicine.medicineId, medicine.name, medicine.category,
                  &medicine.quantity, &medicine.price, medicine.expiryDate,
                  medicine.manufacturer, medicine.description) == 8) {
        if (strstr(medicine.name, searchName) != NULL) {
            showMedicine(&medicine);
            found = 1;
        }
    }

    if (!found) {
        printf("No medicines found matching '%s'.\n", searchName);
    }

    fclose(fp);
    printf("Press Enter to return to menu...");
    getchar();
}

void medicineInventoryLookup() {
    initializeMaxMedicineId();
    int choice;

    while (1) {
        system("cls");
        printf("==== Medicine Inventory ====\n\n");
        printf("1. Add Medicine\n");
        printf("2. Search Medicine\n");
        printf("3. List All Medicines\n");
        printf("4. Update Stock\n");
        printf("5. Low Stock Alert\n");
        printf("6. Delete Medicine\n");
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
                Medicine medicine = makeMedicine();
                makeMedicineEntry(&medicine);
                break;
            case 2:
                searchMedicine();
                break;
            case 3:
                listAllMedicines();
                break;
            case 4:
                updateMedicineStock();
                break;
            case 5:
                listLowStockMedicines();
                break;
            case 6: {
                int medicineId;
                printf("Enter Medicine ID to delete: ");
                scanf("%d", &medicineId);
                getchar();
                deleteMedicine(medicineId);
                break;
            }
            case 7:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}//
// Created by User on 07-Jul-25.
//
