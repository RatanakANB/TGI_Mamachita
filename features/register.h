#ifndef REGISTER_H
#define REGISTER_H
#include <stdio.h>
#include <string.h>
 // Include menu.h instead of earlymenu.h
// #include "menu.h"

int is_user_exists(const char *username) {
    FILE *file = fopen("../data/users.txt", "r");
    if (file == NULL) {
        return 0; // Assume no file means user doesn't exist
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        char stored_user[50];
        if (sscanf(line, "%49[^:]:%*s", stored_user) == 1) {
            if (strcmp(stored_user, username) == 0) {
                fclose(file);
                return 1; // User exists
            }
        }
    }
    fclose(file);
    return 0; // User does not exist
}

int register_user() {
    char username[50], password[50];
    printf("\nRegister\n");
    printf("Enter 'back' to return to menu.\n");
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    if (strcmp(username, "back") == 0) {
        return 0;
    }

    if (is_user_exists(username)) {
        printf("Username already exists. Please choose a different one.\n");
        return 0;
    }

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';
    if (strcmp(password, "back") == 0) {
        return 0;
    }

    FILE *file = fopen("../data/users.txt", "a");
    if (file == NULL) {
        printf("Error opening users.txt for registration.\n");
        return 0;
    }

    fprintf(file, "%s:%s\n", username, password);
    fclose(file);
    printf("Registration successful! You can now log in.\n");
    printf("Returning to menu...\n");
    return 0; // Return to menu, not earlymenu()
}

#endif