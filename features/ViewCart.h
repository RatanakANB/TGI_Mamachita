#ifndef VIEWCART_H
#define VIEWCART_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"
#include "Utils.h"
#include "Checkout.h"

int view_cart(const char *userID) {
    if (!userID) {
        printf("Invalid user information provided.\n");
        return 0;
    }
    // Read cart.txt
    FILE *cart_file = fopen("../data/cart.txt", "r");
    if (!cart_file) {
        printf("No cart items found. The cart.txt file does not exist or cannot be opened.\n");
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    // Count cart items for user
    int cart_count = 0;
    char line[256];
    CartItem temp_cart;
    while (fgets(line, sizeof(line), cart_file)) {
        int parsed = sscanf(line, "%19[^,],%9[^,],%19[^,],%d,%17[^\n]",
                            temp_cart.userID, temp_cart.productID, temp_cart.shopID,
                            &temp_cart.qty, temp_cart.datetime);
        if (parsed == 5 && strcmp(temp_cart.userID, userID) == 0) {
            cart_count++;
        }
    }
    rewind(cart_file);
    if (cart_count == 0) {
        printf("Your cart is empty.\n");
        fclose(cart_file);
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    // Load cart items
    CartItem *cart_items = (CartItem *)malloc(cart_count * sizeof(CartItem));
    if (!cart_items) {
        perror("Error allocating memory for cart items.");
        fclose(cart_file);
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    char **product_names = (char **)malloc(cart_count * sizeof(char *));
    char **shop_names = (char **)malloc(cart_count * sizeof(char *));
    if (!product_names || !shop_names) {
        perror("Error allocating memory for names.");
        free(cart_items);
        if (product_names) free(product_names);
        if (shop_names) free(shop_names);
        fclose(cart_file);
        return 0;
    }
    for (int i = 0; i < cart_count; i++) {
        product_names[i] = (char *)malloc(MAX_ITEM_NAME_LEN * sizeof(char));
        shop_names[i] = (char *)malloc(MAX_SHOP_NAME_LEN * sizeof(char));
        if (!product_names[i] || !shop_names[i]) {
            perror("Error allocating memory for name.");
            for (int j = 0; j < i; j++) {
                free(product_names[j]);
                free(shop_names[j]);
            }
            free(product_names);
            free(shop_names);
            free(cart_items);
            fclose(cart_file);
            return 0;
        }
        strcpy(product_names[i], "Unknown");
        strcpy(shop_names[i], "Unknown");
    }
    int index = 0;
    // Read items.txt to get product and shop names
    FILE *items_file = fopen("../data/items.txt", "r");
    if (!items_file) {
        printf("Error accessing items.txt.\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        fclose(cart_file);
        return 0;
    }
    while (fgets(line, sizeof(line), cart_file)) {
        int parsed = sscanf(line, "%19[^,],%9[^,],%19[^,],%d,%17[^\n]",
                            temp_cart.userID, temp_cart.productID, temp_cart.shopID,
                            &temp_cart.qty, temp_cart.datetime);
        if (parsed == 5 && strcmp(temp_cart.userID, userID) == 0) {
            cart_items[index] = temp_cart;
            // Find product and shop name
            char item_line[512];
            Item item;
            rewind(items_file);
            while (fgets(item_line, sizeof(item_line), items_file)) {
                char temp_datetime[MAX_DATETIME_LEN] = "";
                int item_parsed = sscanf(item_line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                                        item.productID, item.storeID, item.storeName, item.name,
                                        &item.qty, &item.price, item.expir, item.category,
                                        item.manufacturing, item.country, item.distributor, temp_datetime);
                if ((item_parsed == 11 || item_parsed == 12) &&
                    strcmp(item.productID, temp_cart.productID) == 0 &&
                    strcmp(item.storeID, temp_cart.shopID) == 0) {
                    strncpy(product_names[index], item.name, MAX_ITEM_NAME_LEN - 1);
                    strncpy(shop_names[index], item.storeName, MAX_SHOP_NAME_LEN - 1);
                    break;
                }
            }
            index++;
        }
    }
    fclose(cart_file);
    fclose(items_file);
    // Display cart
    printf("\n=== Your Cart ===\n");
    printf("----------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-5s | %-15s |\n", "NO", "PRODUCT NAME", "QTY", "SHOP NAME");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < cart_count; i++) {
        printf("| %-5d | %-20s | %-5d | %-15s |\n", i + 1, product_names[i], cart_items[i].qty, shop_names[i]);
    }
    printf("----------------------------------------------------------\n");
    // Get user action
    char input[100];
    printf("\nOptions:\n");
    printf("1. Modify Quantity\n");
    printf("2. Checkout\n");
    int result = get_validated_input("Enter choice (1-2, or 'back' to return): ", input, sizeof(input),
                                    is_numeric, "Please enter a valid number.");
    if (result <= 0) {
        if (result == -1) printf("Returning to customer dashboard...\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        return 0;
    }
    int choice = atoi(input);
    if (choice == 2) {
        // Proceed to checkout
        checkout(userID);
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        printf("Returning to customer dashboard...\n");
        return 0;
    } else if (choice != 1) {
        printf("Invalid choice. Please enter 1 or 2.\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    // Modify Quantity: Select item
    char prompt[100];
    snprintf(prompt, sizeof(prompt), "Enter item number to modify (1-%d): ", cart_count);
    result = get_validated_input(prompt, input, sizeof(input),
                                is_numeric, "Please enter a valid number.");
    if (result <= 0) {
        if (result == -1) printf("Returning to customer dashboard...\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        return 0;
    }
    int item_index = atoi(input) - 1;
    if (item_index < 0 || item_index >= cart_count) {
        printf("Invalid item number. Please select a number between 1 and %d.\n", cart_count);
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        return 0;
    }
    // Get modification action
    printf("\nSelect action for '%s' from '%s':\n", product_names[item_index], shop_names[item_index]);
    printf("1. Change quantity\n");
    printf("2. Remove from cart\n");
    printf("3. Cancel\n");
    result = get_validated_input("Enter choice (1-3): ", input, sizeof(input),
                                is_numeric, "Please enter a valid number.");
    if (result <= 0 || atoi(input) == 3) {
        if (result == -1 || atoi(input) == 3) printf("Returning to customer dashboard...\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        return 0;
    }
    int sub_choice = atoi(input);
    if (sub_choice != 1 && sub_choice != 2) {
        printf("Invalid choice. Returning to customer dashboard...\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        return 0;
    }
    if (sub_choice == 1) {
        // Change quantity
        items_file = fopen("../data/items.txt", "r");
        if (!items_file) {
            printf("Error accessing items.txt.\n");
            for (int i = 0; i < cart_count; i++) {
                free(product_names[i]);
                free(shop_names[i]);
            }
            free(product_names);
            free(shop_names);
            free(cart_items);
            return 0;
        }
        Item item;
        int stock_qty = 0;
        while (fgets(line, sizeof(line), items_file)) {
            char temp_datetime[MAX_DATETIME_LEN] = "";
            int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                               item.productID, item.storeID, item.storeName, item.name,
                               &item.qty, &item.price, item.expir, item.category,
                               item.manufacturing, item.country, item.distributor, temp_datetime);
            if ((parsed == 11 || parsed == 12) &&
                strcmp(item.productID, cart_items[item_index].productID) == 0 &&
                strcmp(item.storeID, cart_items[item_index].shopID) == 0) {
                stock_qty = item.qty;
                break;
            }
        }
        fclose(items_file);
        result = get_validated_input("Enter new quantity (or 'back' to cancel): ", input, sizeof(input),
                                    is_numeric, "Quantity must be a valid number.");
        if (result <= 0) {
            if (result == -1) printf("Returning to customer dashboard...\n");
            for (int i = 0; i < cart_count; i++) {
                free(product_names[i]);
                free(shop_names[i]);
            }
            free(product_names);
            free(shop_names);
            free(cart_items);
            return 0;
        }
        int new_qty = atoi(input);
        if (new_qty <= 0 || new_qty > stock_qty) {
            printf("Invalid quantity. Must be between 1 and %d.\n", stock_qty);
            for (int i = 0; i < cart_count; i++) {
                free(product_names[i]);
                free(shop_names[i]);
            }
            free(product_names);
            free(shop_names);
            free(cart_items);
            return 0;
        }
        // Update quantity in memory
        cart_items[item_index].qty = new_qty;
    } else if (sub_choice == 2) {
        // Remove item (mark for exclusion)
        cart_items[item_index].qty = -1; // Flag for removal
    }
    // Update cart.txt
    cart_file = fopen("../data/cart.txt", "r");
    if (!cart_file) {
        perror("Error opening cart.txt");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        return 0;
    }
    FILE *temp_file = fopen("../data/cart_temp.txt", "w");
    if (!temp_file) {
        perror("Error creating temporary file");
        fclose(cart_file);
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(cart_items);
        return 0;
    }
    int remaining_count = 0;
    int updated = 0;
    while (fgets(line, sizeof(line), cart_file)) {
        int parsed = sscanf(line, "%19[^,],%9[^,],%19[^,],%d,%17[^\n]",
                            temp_cart.userID, temp_cart.productID, temp_cart.shopID,
                            &temp_cart.qty, temp_cart.datetime);
        if (parsed == 5) {
            int write = 1;
            for (int i = 0; i < cart_count; i++) {
                if (strcmp(temp_cart.userID, cart_items[i].userID) == 0 &&
                    strcmp(temp_cart.productID, cart_items[i].productID) == 0 &&
                    strcmp(temp_cart.shopID, cart_items[i].shopID) == 0) {
                    if (cart_items[i].qty == -1) {
                        write = 0; // Skip removed item
                        updated = 1;
                    } else {
                        // Write updated or unchanged item
                        fprintf(temp_file, "%s,%s,%s,%d,%s\n",
                                temp_cart.userID, temp_cart.productID, temp_cart.shopID,
                                cart_items[i].qty, temp_cart.datetime);
                        write = 0;
                        remaining_count++;
                        updated = 1;
                    }
                    break;
                }
            }
            if (write) {
                // Write non-user items
                fprintf(temp_file, "%s", line);
                if (strcmp(temp_cart.userID, userID) == 0) {
                    remaining_count++;
                }
            }
        }
    }
    fclose(cart_file);
    fclose(temp_file);
    if (updated) {
        if (remove("../data/cart.txt") != 0 || rename("../data/cart_temp.txt", "../data/cart.txt") != 0) {
            perror("Error updating cart.txt");
            remove("../data/cart_temp.txt");
            for (int i = 0; i < cart_count; i++) {
                free(product_names[i]);
                free(shop_names[i]);
            }
            free(product_names);
            free(shop_names);
            free(cart_items);
            return 0;
        }
        printf("✓ Cart updated successfully!\n");
        if (sub_choice == 1) {
            printf("Quantity for '%s' from '%s' changed to %d.\n", product_names[item_index], shop_names[item_index], cart_items[item_index].qty);
        } else {
            printf("Item '%s' from '%s' removed from cart.\n", product_names[item_index], shop_names[item_index]);
        }
    } else {
        remove("../data/cart_temp.txt");
        printf("No changes made to cart.\n");
    }
    // Clean up
    for (int i = 0; i < cart_count; i++) {
        free(product_names[i]);
        free(shop_names[i]);
    }
    free(product_names);
    free(shop_names);
    free(cart_items);
    printf("Returning to customer dashboard...\n");
    return updated;
}

#endif