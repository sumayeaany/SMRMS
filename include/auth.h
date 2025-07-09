//
// Created by User on 08-Jul-25.
//

#ifndef AUTH_H
#define AUTH_H

int authenticateUser(const char* username, const char* password);
void addUser(const char* username, const char* password);
int userExists(const char* username);
void listUsers();
void viewActivityLog();
int loginScreen();
void userManagement();
void createDefaultUser();


#endif //AUTH_H
