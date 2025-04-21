typedef struct 
{
    char name[50];
    int age;
    char number[11];
} patient;

int exists(char name[], int phn);
void show(int phn);
void makeEntry(char name[], int age, int phn);