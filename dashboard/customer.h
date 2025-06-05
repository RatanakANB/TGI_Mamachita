#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <stdio.h>
#include <string.h>
#include "../features/struct.h"
#include "../features/Utils.h"
#include "../features/BrowseShop.h"
#include "../features/ViewCart.h"
#include "../features/Checkout.h"
#include "../features/Invoice.h"

int customerMenu() {
    printf("\n=== CUSTOMER DASHBOARD ===\n");
    printf("1. Browse shops\n");
    printf("2. View cart\n");
    printf("3. Add funds\n\n");
    printf("4. Checkout\n");
    printf("5. View invoices\n");
    printf("6. Return\n");
    printf("Enter choice (1-6): ");
    return 0;
}

int customer_dashboard(const char *userID) {
    if (!userID || strlen(userID) < 10 || strncmp(userID, "CM", 2) != 0) {
        printf("Invalid user ID format. Expected format: CMdd-mm-CUSX (e.g., CM05-06-CUS8).\n");
        printf("Returning to main menu...\n");
        return 0;
    }
    while (1) {
        customerMenu();
        char input[256];
        if (fgets(input, sizeof(input), stdin)) {
            input[strcspn(input, "\n")] = '\0';
            trim_whitespace(input);
            if (strcmp(input, "back") == 0 || strcmp(input, "6") == 0) {
                printf("Returning to main menu...\n");
                return 0;
            }
            if (!is_numeric(input)) {
                printf("Choice must be a numeric value. Please enter a number between 1 and 6.\n");
                continue;
            }
            int choice = atoi(input);
            switch (choice) {
                case 1:
                    browse_shops(userID);
                    break;
                case 2:
                    view_cart(userID);
                    break;
                case 3:
                    printf("Add funds not implemented yet.\n");
                    break;
                case 4:
                    checkout(userID);
                    break;
                case 5:
                    list_invoices(userID); // Call new list_invoices function
                    break;
                case 6:
                    printf("Returning to main menu...\n");
                    return 0;
                default:
                    printf("Invalid choice. Please enter a number between 1 and 6.\n");
                    break;
            }
        } else {
            printf("Error reading input. Please try again.\n");
        }
    }
    return 0;
}

#endif