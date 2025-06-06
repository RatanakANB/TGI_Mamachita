#ifndef ADDITEMS_H
#define ADDITEMS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "struct.h"
#include "Utils.h"

#define MIN_YEAR 2025
#define MAX_YEAR 9999

int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year)) return 29;
    return days[month - 1];
}

int is_valid_expiry(const char *expir) {
    if (!expir || strlen(expir) != 10) return 0;
    if (expir[2] != '-' || expir[5] != '-') return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(expir[i])) return 0;
    }
    int day = atoi(expir);
    int month = atoi(expir + 3);
    int year = atoi(expir + 6);
    if (month < 1 || month > 12) return 0;
    if (year < MIN_YEAR || year > MAX_YEAR) return 0;
    if (day < 1 || day > days_in_month(month, year)) return 0;
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    int current_year = current_time->tm_year + 1900;
    int current_month = current_time->tm_mon + 1;
    int current_day = current_time->tm_mday;
    if (year < current_year || 
        (year == current_year && month < current_month) ||
        (year == current_year && month == current_month && day < current_day)) {
        return 0;
    }
    return 1;
}

int generate_product_id(char *productID) {
    if (!productID) return 0;
    FILE *file = fopen("../data/items.txt", "r");
    int max_id = 0;
    char line[512];
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            char temp_productID[MAX_PRODUCT_ID_LEN];
            char storeID[MAX_ID_LEN];
            char storeName[MAX_SHOP_NAME_LEN];
            char name[MAX_ITEM_NAME_LEN];
            int qty;
            float price;
            char expir[MAX_EXPIRY_LEN];
            char category[MAX_ITEM_NAME_LEN];
            char manufacturing[MAX_MANUFACTURING_LEN];
            char country[MAX_MANUFACTURING_LEN];
            char distributor[MAX_DISTRIBUTOR_LEN];
            char created_datetime[MAX_DATETIME_LEN] = "";
            int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                                temp_productID, storeID, storeName, name, 
                                &qty, &price, expir, category, 
                                manufacturing, country, distributor, created_datetime);
            if (parsed == 11) created_datetime[0] = '\0'; // Old format without datetime
            if ((parsed == 11 || parsed == 12) && strncmp(temp_productID, "PRD", 3) == 0 && strlen(temp_productID) >= 6) {
                int id_num = atoi(temp_productID + 3);
                if (id_num > max_id && id_num < 999) {
                    max_id = id_num;
                }
            }
        }
        fclose(file);
    }
    if (max_id >= 999) {
        return 0;
    }
    snprintf(productID, MAX_PRODUCT_ID_LEN, "PRD%03d", max_id + 1);
    return 1;
}

int add_item(const char *storeID, const char *storeName) {
    if (!storeID || !storeName) {
        printf("Invalid store information provided.\n");
        return 0;
    }
    Item item = {0};
    char input[100];
    printf("\n=== Add New Item ===\n");
    printf("Enter 'back' at any point to return to the shop dashboard.\n\n");
    if (!generate_product_id(item.productID)) {
        printf("Error: Unable to generate product ID. Maximum items reached or system error.\n");
        return 0;
    }
    strncpy(item.storeID, storeID, MAX_ID_LEN - 1);
    strncpy(item.storeName, storeName, MAX_SHOP_NAME_LEN - 1);
    strncpy(item.distributor, storeName, MAX_DISTRIBUTOR_LEN - 1);
    get_current_datetime(item.created_datetime);
    int result = get_validated_input("Enter item name: ", item.name, MAX_ITEM_NAME_LEN, NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    if (is_item_exists(storeID, item.name)) {
        printf("Item '%s' already exists for this shop. Use 'Update Stock' to modify it.\n", item.name);
        printf("Returning to shop dashboard...\n");
        return 0;
    }
    result = get_validated_input("Enter quantity: ", input, sizeof(input), 
                                is_numeric, "Quantity must be a valid number.");
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    item.qty = atoi(input);
    if (item.qty < 0) {
        printf("Quantity cannot be negative. Returning to shop dashboard...\n");
        return 0;
    }
    result = get_validated_input("Enter price: ", input, sizeof(input), 
                                is_valid_float, "Price must be a valid non-negative number (e.g., 10.99).");
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    item.price = atof(input);
    result = get_validated_input("Enter expiry date (DD-MM-YYYY): ", item.expir, MAX_EXPIRY_LEN,
                                is_valid_expiry, "Invalid date format or date is in the past. Use DD-MM-YYYY format.");
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    printf("\nSelect category:\n");
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
        printf("Invalid category choice. Returning to shop dashboard...\n");
        return 0;
    }
    result = get_validated_input("Enter manufacturing details: ", item.manufacturing, 
                                MAX_MANUFACTURING_LEN, NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    printf("\nSelect country of origin:\n");
    printf("1. USA\n2. CHINA\n3. KOREA\n4. KHMER\n");
    result = get_validated_input("Enter choice (1-4): ", input, sizeof(input), NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    const char* countries[] = {"", "USA", "CHINA", "KOREA", "KHMER"};
    int choice = atoi(input);
    if (choice >= 1 && choice <= 4) {
        strcpy(item.country, countries[choice]);
    } else {
        printf("Invalid country choice. Returning to shop dashboard...\n");
        return 0;
    }
    FILE *file = fopen("../data/items.txt", "a");
    if (!file) {
        perror("Error opening items.txt");
        printf("Please ensure the 'data' directory exists and is writable.\n");
        return 0;
    }
    int write_result = fprintf(file, "%s,%s,%s,%s,%d,%.2f,%s,%s,%s,%s,%s,%s\n",
                              item.productID, item.storeID, item.storeName, item.name, 
                              item.qty, item.price, item.expir, item.category, 
                              item.manufacturing, item.country, item.distributor, item.created_datetime);
    if (write_result < 0) {
        perror("Error writing to items.txt");
        fclose(file);
        return 0;
    }
    fclose(file);
    printf("\n✓ Item '%s' added successfully!\n", item.name);
    printf("  Product ID: %s\n", item.productID);
    printf("  Quantity: %d\n", item.qty);
    printf("  Price: %.2f\n", item.price);
    printf("Returning to shop dashboard...\n");
    return 1;
}

#endif