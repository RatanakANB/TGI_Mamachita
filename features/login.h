#ifndef LOGIN_H
#define LOGIN_H
#include <stdio.h>
#include <string.h>
#include "menu.h"

int check_login(const char *username, const char *password) {
    FILE *file = fopen("../data/users.txt", "r");
    if (file == NULL) {
        printf("Error opening users.txt for login.\n");
        return 0;
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        char stored_user[50], stored_pass[50];
        if (sscanf(line, "%49[^:]:%49s", stored_user, stored_pass) == 2) {
            if (strcmp(stored_user, username) == 0 && strcmp(stored_pass, password) == 0) {
                fclose(file);
                return 1; // Valid login
            }
        }
    }
    fclose(file);
    return 0; // Invalid login
}

int login() {
    char username[50], password[50];
    printf("\nLogin\n");
    printf("Enter 'back' to return to menu.\n");
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    if (strcmp(username, "back") == 0) {
        return 0;
    }

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';
    if (strcmp(password, "back") == 0) {
        return 0;
    }

    if (check_login(username, password)) {
        printf("Login successful!\n");
    } else {
        printf("Invalid username or password.\n");
    }
    printf("Returning to menu...\n");
    return 0;
}

#endif