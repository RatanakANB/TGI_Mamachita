#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "struct.h"
#include "Utils.h"
#include "AddItems.h"

int update_stock(const char *storeID, const char *storeName) {
    if (!storeID || !storeName) {
        printf("Invalid store information provided.\n");
        return 0;
    }
    printf("\n=== Update Stock ===\n");
    printf("Enter 'back' to return to the shop dashboard.\n");
    printf("Enter item name or product ID to update.\n\n");
    char input[100];
    char item_name[MAX_ITEM_NAME_LEN] = {0};
    char product_id[MAX_PRODUCT_ID_LEN] = {0};
    int result = get_validated_input("Enter Product items to update: ", input, 
                                    sizeof(input), NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    trim_whitespace(input);
    if (strncmp(input, "PRD", 3) == 0 && strlen(input) == 6) {
        strncpy(product_id, input, MAX_PRODUCT_ID_LEN - 1);
    } else {
        strncpy(item_name, input, MAX_ITEM_NAME_LEN - 1);
    }
    if (item_name[0] && !is_item_exists(storeID, item_name)) {
        printf("Item '%s' does not exist for this shop.\n", item_name);
        printf("Returning to shop dashboard...\n");
        return 0;
    }
    if (product_id[0] && !is_product_id_exists(storeID, product_id)) {
        printf("Product ID '%s' does not exist for this shop.\n", product_id);
        printf("Returning to shop dashboard...\n");
        return 0;
    }
    FILE *file = fopen("../data/items.txt", "r");
    if (!file) {
        perror("Error opening items.txt");
        return 0;
    }
    Item item;
    int found = 0;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char temp_datetime[MAX_DATETIME_LEN] = "";
        int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                            item.productID, item.storeID, item.storeName, item.name, 
                            &item.qty, &item.price, item.expir, item.category, 
                            item.manufacturing, item.country, item.distributor, temp_datetime);
        if (parsed == 11) temp_datetime[0] = '\0'; // Old format without datetime
        if ((parsed == 11 || parsed == 12) && strcmp(item.storeID, storeID) == 0 && 
            (strcasecmp(item.name, item_name) == 0 || strcmp(item.productID, product_id) == 0)) {
            strcpy(item.created_datetime, temp_datetime);
            found = 1;
            break;
        }
    }
    fclose(file);
    if (!found) {
        printf("Error retrieving item details.\n");
        return 0;
    }
    if (strcmp(item.storeName, storeName) != 0) {
        printf("Error: Store name mismatch for item '%s'.\n", item.name);
        return 0;
    }
    printf("\nCurrent Item Details:\n");
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("| %-6s | %-15s | %-20s | %-5s | %-8s | %-10s | %-12s | %-20s | %-7s | %-20s |\n",
           "INDEX", "PRODUCT ID", "NAME", "QTY", "PRICE", "EXPIR", "CATEGORY", "MANUFACTURING", "COUNTRY", "DISTRIBUTOR");
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("| %-6d | %-15s | %-20s | %-5d | $%-7.2f | %-10s | %-12s | %-20s | %-7s | %-20s |\n",
           1, item.productID, item.name, item.qty, item.price, item.expir, item.category,
           item.manufacturing, item.country, item.distributor);
    printf("----------------------------------------------------------------------------------------------------\n");
    while (1) {
        printf("\nSelect field to update:\n");
        printf("1. Product Name\n");
        printf("2. Quantity\n");
        printf("3. Price\n");
        printf("4. Expiry Date\n");
        printf("5. Category\n");
        printf("6. Manufacturing\n");
        printf("7. Country\n");
        printf("8. Done\n");
        result = get_validated_input("Enter choice (1-8): ", input, sizeof(input),
                                    is_numeric, "Choice must be a valid number.");
        if (result <= 0) {
            if (result == -1) printf("Returning to shop dashboard...\n");
            return 0;
        }
        int choice = atoi(input);
        if (choice == 8) {
            printf("No further changes. Saving updates...\n");
            break;
        }
        if (choice < 1 || choice > 8) {
            printf("Invalid choice. Please enter a number between 1 and 8.\n");
            continue;
        }
        switch (choice) {
            case 1:
                result = get_validated_input("Enter new product name: ", input, MAX_ITEM_NAME_LEN, NULL, NULL);
                if (result <= 0) {
                    if (result == -1) printf("Returning to shop dashboard...\n");
                    return 0;
                }
                if (is_item_exists(storeID, input) && strcasecmp(input, item.name) != 0) {
                    printf("Item '%s' already exists for this shop.\n", input);
                    continue;
                }
                strncpy(item.name, input, MAX_ITEM_NAME_LEN - 1);
                printf("Product name updated to '%s'.\n", item.name);
                break;
            case 2:
                result = get_validated_input("Enter new quantity: ", input, sizeof(input),
                                            is_numeric, "Quantity must be a valid number.");
                if (result <= 0) {
                    if (result == -1) printf("Returning to shop dashboard...\n");
                    return 0;
                }
                int new_qty = atoi(input);
                if (new_qty < 0) {
                    printf("Invalid number.\n");
                    continue;
                }
                item.qty = new_qty;
                printf("Quantity updated to %d.\n", item.qty);
                break;
            case 3:
                result = get_validated_input("Enter new price: ", input, sizeof(input),
                                            is_valid_float, "Price must be a valid non-negative number.");
                if (result <= 0) {
                    if (result == -1) printf("Returning to shop dashboard...\n");
                    return 0;
                }
                item.price = atof(input);
                printf("Price updated to $%.2f.\n", item.price);
                break;
            case 4:
                result = get_validated_input("Enter new expiry date (DD-MM-YYYY): ", input, MAX_EXPIRY_LEN,
                                            is_valid_expiry, "Invalid date format or date is in the past. Use DD-MM-YYYY format.");
                if (result <= 0) {
                    if (result == -1) printf("Returning to shop dashboard...\n");
                    return 0;
                }
                strncpy(item.expir, input, MAX_EXPIRY_LEN - 1);
                printf("Expiry date updated to '%s'.\n", item.expir);
                break;
            case 5:
                printf("\nSelect new category:\n");
                printf("1. Grocery\n2. Electronic\n");
                result = get_validated_input("Enter choice (1 or 2): ", input, sizeof(input), NULL, NULL);
                if (result <= 0) {
                    if (result == -1) printf("Returning to shop dashboard...\n");
                    return 0;
                }
                if (strcmp(input, "1") == 0) {
                    strcpy(item.category, "Grocery");
                } else if (strcmp(input, "2") == 0) {
                    strcpy(item.category, "Electronic");
                } else {
                    printf("Invalid category.\n");
                    continue;
                }
                printf("Category updated to '%s'.\n", item.category);
                break;
            case 6:
                result = get_validated_input("Enter new manufacturing details: ", input,
                                            MAX_MANUFACTURING_LEN, NULL, NULL);
                if (result <= 0) {
                    if (result == -1) printf("Returning to shop dashboard...\n");
                    return 0;
                }
                strncpy(item.manufacturing, input, MAX_MANUFACTURING_LEN - 1);
                printf("Manufacturing updated to '%s'.\n", item.manufacturing);
                break;
            case 7:
                printf("\nSelect new country of item\n");
                printf("1. USA\n2. CHINA\n3. KOREA\n4. KHMER\n");
                result = get_validated_input("Enter choice (1-4): ", input, sizeof(input), NULL, NULL);
                if (result <= 0) {
                    if (result == -1) printf("Returning to shop dashboard...\n");
                    return 0;
                }
                const char* countries[] = {"%", "USA", "CHINA", "KOREA", "KHMER"};
                int country_choice = atoi(input);
                if (country_choice >= 1 && country_choice <= 4) {
                    strcpy(item.country, countries[country_choice]);
                    printf("Country updated to '%s'.\n", item.country);
                } else {
                    printf("Invalid country.\n");
                    continue;
                }
                break;
        }
    }
    file = fopen("../data/items.txt", "r");
    if (!file) {
        perror("Error opening items.txt");
        return 0;
    }
    FILE *temp_file = fopen("../data/items_temp.txt", "w");
    if (!temp_file) {
        perror("Error creating temporary file");
        fclose(file);
        return 0;
    }
    int updated = 0;
    while (fgets(line, sizeof(line), file)) {
        Item temp_item;
        char temp_datetime[MAX_DATETIME_LEN] = "";
        int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                           temp_item.productID, temp_item.storeID, temp_item.storeName, temp_item.name, 
                           &temp_item.qty, &temp_item.price, temp_item.expir, temp_item.category, 
                           temp_item.manufacturing, temp_item.country, temp_item.distributor, temp_datetime);
        if (parsed == 11) temp_datetime[0] = '\0';
        strcpy(temp_item.created_datetime, temp_datetime);
        if ((parsed == 11 || parsed == 12) && strcmp(temp_item.storeID, storeID) == 0 && 
            (strcasecmp(temp_item.name, item_name) == 0 || strcmp(temp_item.productID, product_id) == 0)) {
            fprintf(temp_file, "%s,%s,%s,%s,%d,%.2f,%s,%s,%s,%s,%s,%s\n",
                   item.productID, item.storeID, item.storeName, item.name, 
                   item.qty, item.price, item.expir, item.category, 
                   item.manufacturing, item.country, item.distributor, item.created_datetime);
            updated = 1;
        } else {
            fprintf(temp_file, "%s", line);
        }
    }
    fclose(file);
    fclose(temp_file);
    if (updated) {
        if (remove("../data/items.txt") != 0 || 
            rename("../data/items_temp.txt", "../data/items.txt") != 0) {
            perror("Error updating file");
            remove("../data/items_temp.txt");
            return 0;
        }
        printf("✓ Item '%s' updated successfully!\n", item.name);
    } else {
        remove("../data/items_temp.txt");
        printf("Item not found for update.\n");
    }
    printf("Returning to shop dashboard...\n");
    return updated;
}

#endif