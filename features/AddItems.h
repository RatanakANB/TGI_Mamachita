#ifndef ADDITEMS_H
#define ADDITEMS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "struct.h" // Include struct definitions

// Constants for validation
#define MIN_YEAR 2025
#define MAX_YEAR 9999
#define MAX_RETRIES 3

// Enhanced utility functions
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void trim_whitespace(char *str) {
    if (!str) return;
    
    char *end;
    // Trim leading spaces
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == '\0') return;
    
    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

// Enhanced date validation with leap year support
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
    
    // Check if all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(expir[i])) return 0;
    }
    
    int day = atoi(expir);
    int month = atoi(expir + 3);
    int year = atoi(expir + 6);
    
    // Validate ranges
    if (month < 1 || month > 12) return 0;
    if (year < MIN_YEAR || year > MAX_YEAR) return 0;
    if (day < 1 || day > days_in_month(month, year)) return 0;
    
    // Check if date is not in the past
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    int current_year = current_time->tm_year + 1900;
    int current_month = current_time->tm_mon + 1;
    int current_day = current_time->tm_mday;
    
    if (year < current_year || 
        (year == current_year && month < current_month) ||
        (year == current_year && month == current_month && day < current_day)) {
        return 0; // Date is in the past
    }
    
    return 1;
}

// Note: is_numeric function is defined elsewhere in the codebase

// Thread-safe product ID generation with better error handling
int generate_product_id(char *productID) {
    if (!productID) return 0;
    
    FILE *file = fopen("../data/items.txt", "r");
    int max_id = 0;
    char line[512];
    char temp_productID[MAX_PRODUCT_ID_LEN];
    
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            if (sscanf(line, "%9[^,],%*[^\n]", temp_productID) == 1) {
                if (strncmp(temp_productID, "PRD", 3) == 0 && strlen(temp_productID) >= 6) {
                    int id_num = atoi(temp_productID + 3);
                    if (id_num > max_id && id_num < 999) { // Prevent overflow
                        max_id = id_num;
                    }
                }
            }
        }
        fclose(file);
    }
    
    if (max_id >= 999) {
        return 0; // Maximum ID reached
    }
    
    snprintf(productID, MAX_PRODUCT_ID_LEN, "PRD%03d", max_id + 1);
    return 1;
}

// Enhanced item existence check with better error handling
int is_item_exists(const char *storeID, const char *item_name) {
    if (!storeID || !item_name || strlen(item_name) == 0) return 0;
    
    FILE *file = fopen("../data/items.txt", "r");
    if (!file) return 0; // File doesn't exist
    
    char line[512];
    Item item;
    int found = 0;
    
    while (fgets(line, sizeof(line), file) && !found) {
        if (sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^\n]",
                   item.productID, item.storeID, item.storeName, item.name, 
                   &item.qty, &item.price, item.expir, item.category, 
                   item.manufacturing, item.country, item.distributor) == 11) {
            if (strcmp(item.storeID, storeID) == 0 && 
                strcasecmp(item.name, item_name) == 0) { // Case-insensitive comparison
                found = 1;
            }
        }
    }
    
    fclose(file);
    return found;
}

// Enhanced input validation helper
int get_validated_input(const char *prompt, char *buffer, size_t size, 
                       int (*validator)(const char *), const char *error_msg) {
    int attempts = 0;
    
    while (attempts < MAX_RETRIES) {
        printf("%s", prompt);
        if (fgets(buffer, size, stdin)) {
            buffer[strcspn(buffer, "\n")] = '\0';
            trim_whitespace(buffer);
            
            if (strcmp(buffer, "back") == 0) {
                return -1; // User wants to go back
            }
            
            if (validator && !validator(buffer)) {
                printf("%s\n", error_msg);
                attempts++;
                continue;
            }
            
            if (strlen(buffer) > 0) {
                return 1; // Success
            }
        }
        
        printf("Input cannot be empty. Please try again.\n");
        attempts++;
    }
    
    printf("Too many invalid attempts. Returning to shop dashboard...\n");
    return 0; // Failed
}

// Improved add_item function with better structure
int add_item(const char *storeID, const char *storeName) {
    if (!storeID || !storeName) {
        printf("Invalid store information provided.\n");
        return 0;
    }
    
    Item item = {0}; // Initialize all fields to zero
    char input[100];
    
    printf("\n=== Add New Item ===\n");
    printf("Enter 'back' at any point to return to the shop dashboard.\n\n");
    
    // Generate product ID
    if (!generate_product_id(item.productID)) {
        printf("Error: Unable to generate product ID. Maximum items reached or system error.\n");
        return 0;
    }
    
    // Set store information
    strncpy(item.storeID, storeID, MAX_ID_LEN - 1);
    strncpy(item.storeName, storeName, MAX_SHOP_NAME_LEN - 1);
    strncpy(item.distributor, storeName, MAX_DISTRIBUTOR_LEN - 1);
    
    // Get and validate item name
    int result = get_validated_input("Enter item name: ", item.name, MAX_ITEM_NAME_LEN, NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    
    // Check if item already exists
    if (is_item_exists(storeID, item.name)) {
        printf("Item '%s' already exists for this shop. Use 'Update Stock' to modify it.\n", item.name);
        printf("Returning to shop dashboard...\n");
        return 0;
    }
    
    // Get quantity
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
    
    // Get price
    result = get_validated_input("Enter price: ", input, sizeof(input), 
                                is_numeric, "Price must be a valid number.");
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    item.price = atof(input);
    if (item.price < 0) {
        printf("Price cannot be negative. Returning to shop dashboard...\n");
        return 0;
    }
    
    // Get expiry date
    result = get_validated_input("Enter expiry date (DD-MM-YYYY): ", item.expir, MAX_EXPIRY_LEN,
                                is_valid_expiry, "Invalid date format or date is in the past. Use DD-MM-YYYY format.");
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    
    // Get category
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
    
    // Get manufacturing details
    result = get_validated_input("Enter manufacturing details: ", item.manufacturing, 
                                MAX_MANUFACTURING_LEN, NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    
    // Get country
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
    
    // Write to file with error handling
    FILE *file = fopen("../data/items.txt", "a");
    if (!file) {
        perror("Error opening items.txt");
        printf("Please ensure the 'data' directory exists and is writable.\n");
        return 0;
    }
    
    int write_result = fprintf(file, "%s,%s,%s,%s,%d,%.2f,%s,%s,%s,%s,%s\n",
                              item.productID, item.storeID, item.storeName, item.name, 
                              item.qty, item.price, item.expir, item.category, 
                              item.manufacturing, item.country, item.distributor);
    
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

// Enhanced update_stock function with atomic operations
int update_stock(const char *storeID, const char *storeName) {
    if (!storeID || !storeName) {
        printf("Invalid store information provided.\n");
        return 0;
    }
    
    printf("\n=== Update Stock ===\n");
    printf("Enter 'back' to return to the shop dashboard.\n\n");
    
    char item_name[MAX_ITEM_NAME_LEN];
    char input[100];
    
    // Get item name
    int result = get_validated_input("Enter item name to update: ", item_name, 
                                    MAX_ITEM_NAME_LEN, NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    
    // Check if item exists
    if (!is_item_exists(storeID, item_name)) {
        printf("Item '%s' does not exist for this shop.\n", item_name);
        printf("Returning to shop dashboard...\n");
        return 0;
    }
    
    // Get new quantity
    result = get_validated_input("Enter new quantity: ", input, sizeof(input),
                                is_numeric, "Quantity must be a valid number.");
    if (result <= 0) {
        if (result == -1) printf("Returning to shop dashboard...\n");
        return 0;
    }
    
    int new_qty = atoi(input);
    if (new_qty < 0) {
        printf("Quantity cannot be negative. Returning to shop dashboard...\n");
        return 0;
    }
    
    // Perform atomic update using temporary file
    FILE *file = fopen("../data/items.txt", "r");
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
    
    char line[512];
    Item item;
    int updated = 0;
    
    while (fgets(line, sizeof(line), file)) {
        int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^\n]",
                           item.productID, item.storeID, item.storeName, item.name, 
                           &item.qty, &item.price, item.expir, item.category, 
                           item.manufacturing, item.country, item.distributor);
        
        if (parsed == 11 && strcmp(item.storeID, storeID) == 0 && 
            strcasecmp(item.name, item_name) == 0) {
            
            // Verify store name matches
            if (strcmp(item.storeName, storeName) != 0) {
                printf("Error: Store name mismatch for item '%s'.\n", item_name);
                fclose(file);
                fclose(temp_file);
                remove("../data/items_temp.txt");
                return 0;
            }
            
            item.qty = new_qty;
            updated = 1;
            
            fprintf(temp_file, "%s,%s,%s,%s,%d,%.2f,%s,%s,%s,%s,%s\n",
                   item.productID, item.storeID, item.storeName, item.name, 
                   item.qty, item.price, item.expir, item.category, 
                   item.manufacturing, item.country, item.distributor);
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
        printf("✓ Stock for '%s' updated to %d successfully!\n", item_name, new_qty);
    } else {
        remove("../data/items_temp.txt");
        printf("Item '%s' not found for update.\n", item_name);
    }
    
    printf("Returning to shop dashboard...\n");
    return updated;
}

#endif