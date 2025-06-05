#ifndef BROWSESHOP_H
#define BROWSESHOP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../features/struct.h"
#include "../features/Utils.h"
#include "../features/ViewItems.h"

int browse_shops(const char *userID) {
    if (!userID || strlen(userID) < 10 || strncmp(userID, "CM", 2) != 0) {
        printf("Invalid user ID format. Expected format: CMdd-mm-CUSX (e.g., CM05-06-CUS8).\n");
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    FILE *file = fopen("../data/shop.txt", "r");
    if (!file) {
        printf("No shops found. The shop.txt file does not exist or cannot be opened.\n");
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    // Count shops
    int shop_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        shop_count++;
    }
    rewind(file);
    if (shop_count == 0) {
        printf("No shops available.\n");
        fclose(file);
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    // Load shops
    Shop *shops = (Shop *)malloc(shop_count * sizeof(Shop));
    if (!shops) {
        perror("Error allocating memory for shops.");
        fclose(file);
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%19[^,],%99[^\n]", shops[index].id, shops[index].shop_name);
        if (strlen(shops[index].id) < 10 || strncmp(shops[index].id, "SPE", 3) != 0) {
            printf("Invalid shop ID '%s' in shop.txt. Skipping.\n", shops[index].id);
            continue;
        }
        index++;
    }
    shop_count = index; // Adjust for skipped invalid entries
    fclose(file);
    if (shop_count == 0) {
        printf("No valid shops found.\n");
        free(shops);
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    // Display shops
    printf("\n=== Available Shops ===\n");
    printf("-----------------------------\n");
    printf("| %-5s | %-15s |\n", "NO", "SHOP NAME");
    printf("-----------------------------\n");
    for (int i = 0; i < shop_count; i++) {
        printf("| %-5d | %-15s |\n", i + 1, shops[i].shop_name);
    }
    printf("-----------------------------\n");
    // Get shop selection
    char input[100];
    int result = get_validated_input("Enter shop number (or 'back' to return): ", input, sizeof(input),
                                    is_numeric, "Please enter a valid number.");
    if (result <= 0) {
        if (result == -1) printf("Returning to customer dashboard...\n");
        free(shops);
        return 0;
    }
    int shop_index = atoi(input) - 1;
    if (shop_index < 0 || shop_index >= shop_count) {
        printf("Invalid shop number. Please select a number between 1 and %d.\n", shop_count);
        free(shops);
        return 0;
    }
    // View items for selected shop
    view_items(shops[shop_index].id, shops[shop_index].shop_name);
    // Prompt for product ID to add to cart
    printf("\nEnter product ID to add to cart (or 'back' to return): ");
    result = get_validated_input("", input, sizeof(input), NULL, NULL);
    if (result <= 0) {
        if (result == -1) printf("Returning to customer dashboard...\n");
        free(shops);
        return 0;
    }
    trim_whitespace(input);
    // Validate product ID
    FILE *items_file = fopen("../data/items.txt", "r");
    if (!items_file) {
        printf("Error accessing items.txt.\n");
        free(shops);
        return 0;
    }
    Item item;
    int item_found = 0;
    while (fgets(line, sizeof(line), items_file)) {
        char temp_datetime[MAX_DATETIME_LEN] = "";
        int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                           item.productID, item.storeID, item.storeName, item.name,
                           &item.qty, &item.price, item.expir, item.category,
                           item.manufacturing, item.country, item.distributor, temp_datetime);
        if (parsed == 11) temp_datetime[0] = '\0';
        if ((parsed == 11 || parsed == 12) && strcmp(item.storeID, shops[shop_index].id) == 0 &&
            strcmp(item.productID, input) == 0) {
            item_found = 1;
            break;
        }
    }
    fclose(items_file);
    if (!item_found) {
        printf("Product ID '%s' not found for shop '%s'.\n", input, shops[shop_index].shop_name);
        free(shops);
        return 0;
    }
    // Get quantity
    result = get_validated_input("Enter quantity (or 'back' to return): ", input, sizeof(input),
                                is_numeric, "Quantity must be a valid number.");
    if (result <= 0) {
        if (result == -1) printf("Returning to customer dashboard...\n");
        free(shops);
        return 0;
    }
    int qty = atoi(input);
    if (qty <= 0 || qty > item.qty) {
        printf("Invalid quantity. Must be between 1 and %d.\n", item.qty);
        free(shops);
        return 0;
    }
    // Add to cart
    CartItem cart_item = {0};
    strncpy(cart_item.userID, userID, MAX_ID_LEN - 1); // Use userID (e.g., CM05-06-CUS8)
    strncpy(cart_item.productID, item.productID, MAX_PRODUCT_ID_LEN - 1);
    strncpy(cart_item.shopID, item.storeID, MAX_ID_LEN - 1);
    cart_item.qty = qty;
    get_current_datetime(cart_item.datetime);
    FILE *cart_file = fopen("../data/cart.txt", "a");
    if (!cart_file) {
        perror("Error opening cart.txt");
        free(shops);
        printf("Please ensure the 'data' directory exists and is writable.\n");
        return 0;
    }
    int write_result = fprintf(cart_file, "%s,%s,%s,%d,%s\n",
                              cart_item.userID, cart_item.productID, cart_item.shopID,
                              cart_item.qty, cart_item.datetime);
    if (write_result < 0) {
        perror("Error writing to cart.txt");
        fclose(cart_file);
        free(shops);
        return 0;
    }
    fclose(cart_file);
    printf("✓ Item '%s' (Qty: %d) added to cart successfully!\n", item.name, qty);
    free(shops);
    printf("Returning to customer dashboard...\n");
    return 1;
}

#endif