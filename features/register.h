#ifndef REGISTER_H
#define REGISTER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "encryption.h"
#include "struct.h" // Include the struct definitions
#include "Utils.h"  // Include get_current_datetime

#define USER_TYPE_ADMIN 0
#define USER_TYPE_SHOP  1
#define USER_TYPE_CUSTOMER 2

int is_user_exists(const char *username);
int register_user();
int count_users();
static inline void regiType();

// Function to generate UniqueID for Shop
void generate_shop_id(char *unique_id, const char *shop_name) {
    char datetime[13];
    get_current_datetime(datetime); // Get dynamic datetime
    char date[7];
    strncpy(date, datetime, 6); // Extract ddmmyy
    date[6] = '\0';
    int len = strlen(shop_name);
    char middle = len > 0 ? shop_name[len / 2] : 'X'; // Middle character, default 'X'
    char first = len > 0 ? shop_name[0] : 'X'; // First character, default 'X'
    char last = len > 0 ? shop_name[len - 1] : 'X'; // Last character, default 'X'
    snprintf(unique_id, MAX_ID_LEN, "SP%c%c%c%s", first, last, middle, date);
}

// Function to generate UniqueID for Customer
void generate_customer_id(char *unique_id, const char *username) {
    char datetime[13];
    get_current_datetime(datetime); // Get dynamic datetime
    char date[7];
    strncpy(date, datetime, 6); // Extract ddmmyy
    date[6] = '\0';
    char user_part[4] = "___"; // Default padding with underscores
    int len = strlen(username);
    for (int i = 0; i < 3 && i < len; i++) {
        user_part[i] = username[i]; // Take first 3 characters
    }
    srand(time(NULL)); // Seed random number generator
    int random_digit = rand() % 10; // Random digit 0-9
    snprintf(unique_id, MAX_ID_LEN, "CM%s%s%d", date, user_part, random_digit);
}

// Function to generate UniqueID for Admin
void generate_admin_id(char *unique_id, int user_count) {
    snprintf(unique_id, MAX_ID_LEN, "AD%d", user_count + 1); // Simple counter-based ID
}

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
    userName user;
    while (fgets(line, sizeof(line), file) != NULL) {
        int parsed = sscanf(line, "%19[^,],%49[^,],%49[^,],%d,%12[^,\n]", 
                           user.id, user.username, user.password, &user.usertype, user.creation_datetime);
        if (parsed >= 4 && strcmp(user.username, username) == 0) { 
            if (parsed < 5) user.creation_datetime[0] = '\0'; // Backward compatibility
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int register_user() {
    userName user;
    Shop shop;
    char user_type_choice[10];
    float default_balance = 9000.00; // Default balance for customers

    printf("\n--- Register ---\n");
    printf("Enter 'back' at any point to return to the main menu.\n\n");

    int existing_users_count = count_users();

    while (1) {
        printf("Enter your desired username: ");
        fgets(user.username, MAX_USERNAME_LEN, stdin);
        user.username[strcspn(user.username, "\n")] = '\0'; 
        if (strcmp(user.username, "back") == 0) {
            printf("Returning to menu...\n");
            return 0;
        }
        if (strlen(user.username) == 0) {
            printf("Username cannot be empty. Please try again.\n");
            continue;
        }
        if (is_user_exists(user.username)) {
            printf("Username '%s' already exists. Please choose a different one.\n", user.username);
        } else {
            break;
        }
    }

    printf("Enter your desired password: ");
    fgets(user.password, MAX_PASSWORD_LEN, stdin);
    user.password[strcspn(user.password, "\n")] = '\0'; 
    if (strcmp(user.password, "back") == 0) {
        printf("Returning to menu...\n");
        return 0;
    }
    if (strlen(user.password) == 0) {
        printf("Password cannot be empty. Registration cancelled.\n");
        printf("Returning to menu...\n");
        return 0;
    }

    encrypt_password(user.password); // Encrypt the password in place

    // Set creation datetime
    get_current_datetime(user.creation_datetime);

    if (existing_users_count == 0) {
        user.usertype = USER_TYPE_ADMIN;
        printf("This is the first user account. Registering as ADMIN.\n");
        generate_admin_id(user.id, existing_users_count);
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
                user.usertype = USER_TYPE_SHOP;
                printf("Enter your shop name: ");
                fgets(shop.shop_name, MAX_SHOP_NAME_LEN, stdin);
                shop.shop_name[strcspn(shop.shop_name, "\n")] = '\0';
                if (strcmp(shop.shop_name, "back") == 0) {
                    printf("Returning to menu...\n");
                    return 0;
                }
                if (strlen(shop.shop_name) == 0) {
                    printf("Shop name cannot be empty. Registration cancelled.\n");
                    printf("Returning to menu...\n");
                    return 0;
                }
                generate_shop_id(user.id, shop.shop_name);
                strcpy(shop.id, user.id); // Inherit ID from user
                break;
            } else if (choice == 2) {
                user.usertype = USER_TYPE_CUSTOMER;
                shop.shop_name[0] = '\0'; // No shop name for customers
                generate_customer_id(user.id, user.username);
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

    fprintf(file, "%s,%s,%s,%d,%s\n", user.id, user.username, user.password, user.usertype, user.creation_datetime);
    fclose(file);

    if (user.usertype == USER_TYPE_SHOP) {
        FILE *shop_file = fopen("../data/shop.txt", "a");
        if (shop_file == NULL) {
            perror("Error opening shop.txt for registration. Please ensure the 'data' directory exists.");
            printf("Returning to menu...\n");
            return 0;
        }
        fprintf(shop_file, "%s,%s\n", shop.id, shop.shop_name);
        fclose(shop_file);
    } else if (user.usertype == USER_TYPE_CUSTOMER) {
        FILE *customer_file = fopen("../data/customer.txt", "a");
        if (customer_file == NULL) {
            perror("Error opening customer.txt for registration. Please ensure the 'data' directory exists.");
            printf("Returning to menu...\n");
            return 0;
        }
        fprintf(customer_file, "%s,%s,%.2f\n", user.id, user.username, default_balance);
        fclose(customer_file);
    }

    const char *type_string;
    switch (user.usertype) {
        case USER_TYPE_ADMIN: type_string = "ADMIN"; break;
        case USER_TYPE_SHOP: type_string = "Shop"; break;
        case USER_TYPE_CUSTOMER: type_string = "Customer"; break;
        default: type_string = "Unknown"; break;
    }

    printf("Registration successful for user '%s' as a %s with ID '%s' on %s!\n", 
           user.username, type_string, user.id, user.creation_datetime);
    printf("You can now log in.\n");
    printf("Returning to menu...\n");
    return 0; 
}

#endif