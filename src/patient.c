#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "patient.h"

#define DATAFILE "data/patient.csv"

int exists(char name[], char phn[]) {
    FILE *fp = fopen(DATAFILE, "r");
    if (!fp) {
        perror("Unable to open data file");
        return 0;
    }

    char line[128];

    while (fgets(line, sizeof(line), fp)) {
        char *tok_name = strtok(line, ",");
        char *tok_age  = strtok(NULL, ",");
        char *tok_phn  = strtok(NULL, ",\n");
        if (!tok_name || !tok_age || !tok_phn) continue;

        if (strcmp(tok_name, name) == 0 || strcmp(tok_phn, phn) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void show(char phn[]) {
    FILE *fp = fopen(DATAFILE, "r");
    if (!fp) {
        perror("Unable to open data file");
        return;
    }

    char line[128];
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        char *tok_name = strtok(line, ",");
        char *tok_age  = strtok(NULL, ",");
        char *tok_phn  = strtok(NULL, ",\n");
        if (!tok_name || !tok_age || !tok_phn) continue;

        if (strcmp(tok_phn, phn) == 0) {
            printf("\n==== Patient Information ====\n");
            printf("Name: %s\n", tok_name);
            printf("Age : %s\n", tok_age);
            printf("Phone: %s\n", tok_phn);
            break;
        }
    }
    printf("Press Enter to return to menu...");
    getchar(); getchar();

    fclose(fp);
}

void makeEntry(char name[], int age, char phn[]) {
    if (exists(name, phn)) {
        printf("A patient with this name or phone already exists.\n");
    } else {
        FILE *fp = fopen(DATAFILE, "a");
        if (!fp) {
            perror("Unable to open data file");
            return;
        }
        fprintf(fp, "%s,%d,%s\n", name, age, phn);
        fclose(fp);
        printf("Info added!\n");
    }
    printf("Press Enter to return to menu...");
    getchar(); getchar();
}
