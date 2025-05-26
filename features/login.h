#ifndef LOGIN_H
#define LOGIN_H

#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "encryption.h" // Include the encryption header
#include "../dashboard/shop.h"
#include "../dashboard/customer.h"
#include "../dashboard/admin.h"

#define USER_TYPE_ADMIN 0
#define USER_TYPE_SHOP  1
#define USER_TYPE_CUSTOMER 2

int check_login(const char *username, const char *password, int *user_type_ptr);
int login();

int check_login(const char *username, const char *password, int *user_type_ptr) {
    FILE *file = fopen("../data/users.txt", "r");
    if (file == NULL) {
        perror("Error opening users.txt for login. Please ensure the 'data' directory exists and the file is accessible.");
        return -1;
    }

    char line[150]; 
    if (user_type_ptr) {
        *user_type_ptr = -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char stored_user[50], stored_pass[50];
        int stored_type = -1;

        if (sscanf(line, "%49[^:]:%49[^:]:%d", stored_user, stored_pass, &stored_type) == 3) {
            if (strcmp(stored_user, username) == 0 && strcmp(stored_pass, password) == 0) {
                fclose(file);
                if (user_type_ptr) {
                    *user_type_ptr = stored_type; 
                }
                return stored_type; 
            }
        }
    }
    fclose(file);
    return -1;
}

int login() {
    char username[50], password[50];
    int user_type_from_file = -1; 

    printf("\n--- Login ---\n");
    printf("Enter 'back' at any point to return to the main menu.\n\n");

    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; 
    if (strcmp(username, "back") == 0) {
        printf("Returning to menu...\n");
        return 0; 
    }
    if (strlen(username) == 0) {
        printf("Username cannot be empty. Please try again.\n");
        printf("Returning to menu...\n");
        return 0;
    }

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0'; 
    if (strcmp(password, "back") == 0) {
        printf("Returning to menu...\n");
        return 0; 
    }
    if (strlen(password) == 0) {
        printf("Password cannot be empty. Login failed.\n");
        printf("Returning to menu...\n");
        return 0;
    }

    encrypt_password(password); // Encrypt the entered password
    int login_status = check_login(username, password, &user_type_from_file);

    if (login_status >= 0) { 
        printf("Login successful! Welcome, %s.\n", username);
        switch (user_type_from_file) {
            case USER_TYPE_ADMIN:
                printf("User Type: ADMIN\n");
                admin_dashboard("DummyAdminName"); // Replace with actual admin name if needed
                break;
            case USER_TYPE_SHOP:
                printf("User Type: Shop\n");
                shop_dashboard("DummyShopName"); // Replace with actual shop name if needed
                break;
            case USER_TYPE_CUSTOMER:
                printf("User Type: Customer\n");
                customer_dashboard("DummyCustomerName"); // Replace with actual customer name if needed
                break;
            default:
                printf("User type (%d) is recognized but not specifically handled after login.\n", user_type_from_file);
                break;
        }
    } else {
        printf("Invalid username or password. Please try again.\n");
    }

    return 0; 
}

#endif