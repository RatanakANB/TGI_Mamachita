#ifndef LOGIN_H
#define LOGIN_H

#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "encryption.h"
#include "struct.h" // Include the struct definitions
#include "../dashboard/shop.h"
#include "../dashboard/customer.h"
#include "../dashboard/admin.h"

#define USER_TYPE_ADMIN 0
#define USER_TYPE_SHOP  1
#define USER_TYPE_CUSTOMER 2

int check_login(const char *username, const char *password, userName *user);
int login();

int check_login(const char *username, const char *password, userName *user) {
    FILE *file = fopen("../data/users.txt", "r");
    if (file == NULL) {
        perror("Error opening users.txt for login. Please ensure the 'data' directory exists.");
        return -1;
    }

    char line[150]; 
    userName temp_user;
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%19[^,],%49[^,],%49[^,],%d", 
                   temp_user.id, temp_user.username, temp_user.password, &temp_user.usertype) == 4) {
            if (strcmp(temp_user.username, username) == 0 && strcmp(temp_user.password, password) == 0) {
                strcpy(user->id, temp_user.id);
                strcpy(user->username, temp_user.username);
                strcpy(user->password, temp_user.password);
                user->usertype = temp_user.usertype;
                fclose(file);
                return temp_user.usertype;
            }
        }
    }
    fclose(file);
    return -1;
}

int login() {
    userName user;
    char password_input[MAX_PASSWORD_LEN];
    Shop shop;

    printf("\n--- Login ---\n");
    printf("Enter 'back' at any point to return to the main menu.\n\n");

    printf("Enter your username: ");
    fgets(user.username, MAX_USERNAME_LEN, stdin);
    user.username[strcspn(user.username, "\n")] = '\0'; 
    if (strcmp(user.username, "back") == 0) {
        printf("Returning to menu...\n");
        return 0; 
    }
    if (strlen(user.username) == 0) {
        printf("Username cannot be empty. Please try again.\n");
        printf("Returning to menu...\n");
        return 0;
    }

    printf("Enter your password: ");
    fgets(password_input, MAX_PASSWORD_LEN, stdin);
    password_input[strcspn(password_input, "\n")] = '\0'; 
    if (strcmp(password_input, "back") == 0) {
        printf("Returning to menu...\n");
        return 0; 
    }
    if (strlen(password_input) == 0) {
        printf("Password cannot be empty. Login failed.\n");
        printf("Returning to menu...\n");
        return 0;
    }

    encrypt_password(password_input); // Encrypt the input password
    int login_status = check_login(user.username, password_input, &user);

    if (login_status >= 0) {
        printf("Login successful! Welcome, %s.\n", user.username);
        char name[MAX_SHOP_NAME_LEN] = {0};

        if (user.usertype == USER_TYPE_SHOP) {
            FILE *shop_file = fopen("../data/shop.txt", "r");
            if (shop_file) {
                char line[150];
                while (fgets(line, sizeof(line), shop_file)) {
                    if (sscanf(line, "%19[^,],%99[^\n]", shop.id, shop.shop_name) == 2) {
                        if (strcmp(shop.id, user.id) == 0) {
                            strcpy(name, shop.shop_name);
                            break;
                        }
                    }
                }
                fclose(shop_file);
            }
            if (strlen(name) == 0) strcpy(name, user.username); // Fallback to username
        } else {
            strcpy(name, user.username); // Use username for admin/customer
        }

        switch (user.usertype) {
            case USER_TYPE_ADMIN:
                printf("User Type: ADMIN\n");
                admin_dashboard(name);
                break;
            case USER_TYPE_SHOP:
                printf("User Type: Shop\n");
                shop_dashboard(user.id, name); // Pass both storeID and storeName
                break;
            case USER_TYPE_CUSTOMER:
                printf("User Type: Customer\n");
                customer_dashboard(name);
                break;
            default:
                printf("User type (%d) is recognized but not specifically handled after login.\n", user.usertype);
                break;
        }
    } else {
        printf("Invalid username or password. Please try again.\n");
    }

    return 0; 
}

#endif