#ifndef REGISTER_H
#define REGISTER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encryption.h" // Include the encryption header

#define USER_TYPE_ADMIN 0
#define USER_TYPE_SHOP  1
#define USER_TYPE_CUSTOMER 2



int is_user_exists(const char *username);
int register_user();
int count_users();
static inline void regiType();


static inline void regiType() {
    printf("Select user type:\n");
    printf("  1. Shop\n");
    printf("  2. Customer\n");
    printf("Enter your choice (1 or 2): ");
}


int count_users() {
    FILE *file = fopen("../data/users.txt", "r");
    if (file == NULL) {
        return 0;
    }

    int count = 0;
    char line[150];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) > 1 && line[0] != '\n') {
            count++;
        }
    }
    fclose(file);
    return count;
}

int is_user_exists(const char *username) {
    FILE *file = fopen("../data/users.txt", "r");
    if (file == NULL) {
        return 0; 
    }

    char line[150]; 
    while (fgets(line, sizeof(line), file) != NULL) {
        char stored_user[50];
        if (sscanf(line, "%49[^:]:", stored_user) == 1) { 
            if (strcmp(stored_user, username) == 0) {
                fclose(file);
                return 1;
            }
        }
    }
    fclose(file);
    return 0;
}

int register_user() {
    char username[50], password[50], user_type_choice[10];
    int user_type;

    printf("\n--- Register ---\n");
    printf("Enter 'back' at any point to return to the main menu.\n\n");

    int existing_users_count = count_users();

    while (1) {
        printf("Enter your desired username: ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = '\0'; 
        if (strcmp(username, "back") == 0) {
            printf("Returning to menu...\n");
            return 0;
        }
        if (strlen(username) == 0) {
            printf("Username cannot be empty. Please try again.\n");
            continue;
        }
        if (is_user_exists(username)) {
            printf("Username '%s' already exists. Please choose a different one.\n", username);
        } else {
            break;
        }
    }

    printf("Enter your desired password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0'; 
    if (strcmp(password, "back") == 0) {
        printf("Returning to menu...\n");
        return 0;
    }
    if (strlen(password) == 0) {
        printf("Password cannot be empty. Registration cancelled.\n");
        printf("Returning to menu...\n");
        return 0;
    }

    encrypt_password(password); // Encrypt the password before storing

    if (existing_users_count == 0) {
        user_type = USER_TYPE_ADMIN;
        printf("This is the first user account. Registering as ADMIN.\n");
    } else {
        while (1) {
            regiType();
            fgets(user_type_choice, sizeof(user_type_choice), stdin);
            user_type_choice[strcspn(user_type_choice, "\n")] = '\0';

            if (strcmp(user_type_choice, "back") == 0) {
                printf("Returning to menu...\n");
                return 0;
            }

            int choice = atoi(user_type_choice); 

            if (choice == 1) {
                user_type = USER_TYPE_SHOP;
                break;
            } else if (choice == 2) {
                user_type = USER_TYPE_CUSTOMER;
                break;
            } else {
                printf("Invalid choice. Please enter 1 for Shop or 2 for Customer.\n");
            }
        }
    }

    FILE *file = fopen("../data/users.txt", "a");
    if (file == NULL) {
        perror("Error opening users.txt for registration. Please ensure the 'data' directory exists.");
        printf("Returning to menu...\n");
        return 0;
    }

    fprintf(file, "%s:%s:%d\n", username, password, user_type);
    fclose(file);

    const char *type_string;
    switch (user_type) {
        case USER_TYPE_ADMIN: type_string = "ADMIN"; break;
        case USER_TYPE_SHOP: type_string = "Shop"; break;
        case USER_TYPE_CUSTOMER: type_string = "Customer"; break;
        default: type_string = "Unknown"; break;
    }

    printf("Registration successful for user '%s' as a %s!\n", username, type_string);
    printf("You can now log in.\n");
    printf("Returning to menu...\n");
    return 0; 
}

#endif