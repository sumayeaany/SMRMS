#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../include/patient.h"

void test_makeEntry_and_exists() {
    makeEntry("Alice", 25, 1234567890);
    assert(exists("Alice", 1234567890) == 1);
}

void test_exists_negative() {
    assert(exists("Ghost", 999999999) == 0);
}

void test_show() {
    printf("Testing show (manual check needed)...\n");
    show(1234567890); // Just runs to see no crash
}

int main() {
    test_makeEntry_and_exists();
    test_exists_negative();
    test_show();
    printf("✅ All tests passed!\n");
    return 0;
}
