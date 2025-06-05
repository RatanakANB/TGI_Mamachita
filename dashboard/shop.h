#ifndef SHOP_H
#define SHOP_H

#include <stdio.h>
#include <string.h>
#include "../features/AddItems.h"
#include "../features/Update.h"
#include "../features/ViewItems.h"

int shopMenu(const char *storeID, const char *storeName);
int shop_dashboard(const char *storeID, const char *storeName);

int shopMenu(const char *storeID, const char *storeName) {
    int choice;
    char input[10];

    printf("\n=== SHOP DASHBOARD ===\n");
    printf("1. Add item\n");
    printf("2. Update stock\n");
    printf("3. Edit shop name\n");
    printf("4. View items\n");
    printf("5. View Logs\n");
    printf("6. Back\n");
    printf("Enter your choice (1-6): ");
    
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "back") == 0) {
        printf("Returning to main menu...\n");
        return 0;
    }
    if (!is_numeric(input)) {
        printf("Choice must be a numeric value. Please enter a number between 1 and 6.\n");
        return 1;
    }
    choice = atoi(input);

    switch (choice) {
        case 1:
            add_item(storeID, storeName);
            break;
        case 2:
            update_stock(storeID, storeName);
            break;
        case 3:
            printf("Edit shop name not implemented yet.\n");
            break;
        case 4:
            view_items(storeID, storeName);
            break;
        case 5:
            printf("View logs not implemented yet.\n");
            break;
        case 6:
            printf("Returning to main menu...\n");
            return 0;
        default:
            printf("Invalid choice. Please enter a number between 1 and 6.\n");
            break;
    }
    return 1; // Continue showing menu unless 'Back' is chosen
}

int shop_dashboard(const char *storeID, const char *storeName) {
    while (shopMenu(storeID, storeName)) {
        // Keep showing menu until user chooses to go back
    }
    return 0;
}

#endif