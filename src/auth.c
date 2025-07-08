//
// Created by User on 08-Jul-25.
//
// src/auth.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auth.h"

#define USERS_FILE "data/users.csv"
#define LOG_FILE "data/activity.log"

static void createDataDirectory() {
    #ifdef _WIN32
        system("if not exist data mkdir data");
    #else
        system("mkdir -p data");
    #endif
}

static void logActivity(const char* username, const char* action) {
    createDataDirectory();

    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) return;

    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);

    fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d - %s: %s\n",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
            username, action);

    fclose(fp);
}

int authenticateUser(const char* username, const char* password) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) {
        printf("No users file found. Please contact administrator.\n");
        return 0;
    }

    char fileUsername[50], filePassword[50];
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        // Remove newline character from line
        line[strcspn(line, "\n")] = '\0';

        // Parse username and password from the line
        char *token = strtok(line, ",");
        if (token) {
            strncpy(fileUsername, token, sizeof(fileUsername) - 1);
            fileUsername[sizeof(fileUsername) - 1] = '\0';

            token = strtok(NULL, ",");
            if (token) {
                strncpy(filePassword, token, sizeof(filePassword) - 1);
                filePassword[sizeof(filePassword) - 1] = '\0';

                if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
                    fclose(fp);
                    logActivity(username, "Logged in successfully");
                    return 1;
                }
            }
        }
    }

    fclose(fp);
    logActivity(username, "Failed login attempt");
    printf("Invalid username or password.\n");
    printf("Press Enter to continue...");
    getchar();
    return 0;
}

void addUser(const char* username, const char* password) {
    createDataDirectory();

    FILE *fp = fopen(USERS_FILE, "a");
    if (!fp) {
        printf("Error creating users file.\n");
        return;
    }

    fprintf(fp, "%s,%s\n", username, password);
    fclose(fp);

    logActivity("ADMIN", "Added new user");
    printf("User added successfully.\n");
}

int userExists(const char* username) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) return 0;

    char fileUsername[50], filePassword[50];
    while (fscanf(fp, "%49[^,],%49[^\n]", fileUsername, filePassword) == 2) {
        if (strcmp(username, fileUsername) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void listUsers() {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) {
        printf("No users found.\n");
        return;
    }

    printf("\n==== Registered Users ====\n");
    char username[50], password[50];
    int count = 0;

    while (fscanf(fp, "%49[^,],%49[^\n]", username, password) == 2) {
        printf("%d. %s\n", ++count, username);
    }

    fclose(fp);
    printf("Total users: %d\n", count);
}

void viewActivityLog() {
    FILE *fp = fopen(LOG_FILE, "r");
    if (!fp) {
        printf("No activity log found.\n");
        return;
    }

    printf("\n==== Activity Log ====\n");
    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < 50) {
        printf("%s", line);
        count++;
    }

    if (count == 50) {
        printf("... (showing last 50 entries)\n");
    }

    fclose(fp);
}

int loginScreen() {
    char username[50], password[50];
    int attempts = 0;
    const int maxAttempts = 3;

    while (attempts < maxAttempts) {
        system("cls");
        printf("==== Smart Medical Record Management System ====\n");

        printf("Username: ");
        scanf("%49s", username);

        printf("Password: ");
        scanf("%49s", password);

        if (authenticateUser(username, password)) {
            printf("\nLogin successful! Welcome, %s\n", username);
            printf("Press Enter to continue...");
            getchar();
            return 1;
        }
        attempts++;
        printf("\nInvalid username or password. ");
        printf("Attempts remaining: %d\n", maxAttempts - attempts);
        if (attempts < maxAttempts) {
            printf("Press Enter to try again...");
            getchar();
        }
    }

    printf("\nToo many failed attempts. Access denied.\n");
    logActivity("UNKNOWN", "Multiple failed login attempts - access denied");
    return 0;
}

void userManagement() {
    int choice;

    while (1) {
        system("cls");
        printf("==== User Management ====\n\n");
        printf("1. Add User\n");
        printf("2. List Users\n");
        printf("3. View Activity Log\n");
        printf("4. Back to Main Menu\n");
        printf("\nChoice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n') {}
            printf("Invalid input. Press Enter to continue...");
            getchar();
            continue;
        }
        getchar();

        switch (choice) {
            case 1: {
                char username[50], password[50];
                printf("Enter username: ");
                scanf("%49s", username);

                if (userExists(username)) {
                    printf("User already exists!\n");
                    printf("Press Enter to continue...");
                    getchar();
                    getchar();
                    break;
                }

                printf("Enter password: ");
                scanf("%49s", password);

                addUser(username, password);
                printf("Press Enter to continue...");
                getchar();
                getchar();
                break;
            }
            case 2:
                listUsers();
                printf("Press Enter to continue...");
                getchar();
                break;
            case 3:
                viewActivityLog();
                printf("Press Enter to continue...");
                getchar();
                break;
            case 4:
                return;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
        }
    }
}

void createDefaultUser() {
    FILE *fp = fopen(USERS_FILE, "r");
    if (fp) {
        fclose(fp);
        return; // Users file exists
    }

    createDataDirectory();
    addUser("admin", "admin123");
    logActivity("SYSTEM", "Created default admin user");
}