#ifndef CHECKOUT_H
#define CHECKOUT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "struct.h"
#include "Utils.h"
#include "Invoice.h"

int checkout(const char *userID) {
    if (!userID || strlen(userID) < 10 || strncmp(userID, "CM", 2) != 0) {
        printf("Invalid user ID format. Expected format: CMdd-mm-CUSX (e.g., CM05-06-CUS8).\n");
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    FILE *cart_file = fopen("../data/cart.txt", "r");
    if (!cart_file) {
        printf("No cart items found. The cart.txt file does not exist or cannot be opened.\n");
        return 0;
    }
    int cart_count = 0;
    char line[256];
    CartItem temp_cart;
    while (fgets(line, sizeof(line), cart_file)) {
        int parsed = sscanf(line, "%19[^,],%9[^,],%19[^,],%d,%17[^\n]",
                            temp_cart.userID, temp_cart.productID, temp_cart.shopID,
                            &temp_cart.qty, temp_cart.datetime);
        if (parsed == 5 && strcmp(temp_cart.userID, userID) == 0) {
            if (strlen(temp_cart.shopID) < 8 || strncmp(temp_cart.shopID, "SP", 2) != 0) {
                printf("Invalid shop ID '%s' in cart. Expected format: SPddmmyyXXXX.\n", temp_cart.shopID);
                fclose(cart_file);
                return 0;
            }
            cart_count++;
        }
    }
    rewind(cart_file);
    if (cart_count == 0) {
        printf("Your cart is empty or contains invalid entries.\n");
        fclose(cart_file);
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    CartItem *cart_items = (CartItem *)malloc(cart_count * sizeof(CartItem));
    char **product_names = (char **)malloc(cart_count * sizeof(char *));
    char **shop_names = (char **)malloc(cart_count * sizeof(char *));
    float *unit_prices = (float *)malloc(cart_count * sizeof(float));
    float *totals = (float *)malloc(cart_count * sizeof(float));
    int *stock_quantities = (int *)malloc(cart_count * sizeof(int));
    if (!cart_items || !product_names || !shop_names || !unit_prices || !totals || !stock_quantities) {
        perror("Error allocating memory.");
        free(cart_items);
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        fclose(cart_file);
        return 0;
    }
    for (int i = 0; i < cart_count; i++) {
        product_names[i] = (char *)malloc(MAX_ITEM_NAME_LEN * sizeof(char));
        shop_names[i] = (char *)malloc(MAX_SHOP_NAME_LEN * sizeof(char));
        if (!product_names[i] || !shop_names[i]) {
            perror("Error allocating memory for names.");
            for (int j = 0; j < i; j++) {
                free(product_names[j]);
                free(shop_names[j]);
            }
            free(product_names);
            free(shop_names);
            free(unit_prices);
            free(totals);
            free(stock_quantities);
            free(cart_items);
            fclose(cart_file);
            return 0;
        }
        strcpy(product_names[i], "Unknown");
        strcpy(shop_names[i], "Unknown");
        unit_prices[i] = 0.0;
        totals[i] = 0.0;
        stock_quantities[i] = 0;
    }
    int index = 0;
    FILE *items_file = fopen("../data/items.txt", "r");
    if (!items_file) {
        printf("Error accessing items.txt.\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
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
            char item_line[512];
            Item item;
            rewind(items_file);
            int found = 0;
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
                    unit_prices[index] = item.price;
                    totals[index] = item.price * temp_cart.qty;
                    stock_quantities[index] = item.qty;
                    found = 1;
                    break;
                }
            }
            if (!found || temp_cart.qty > stock_quantities[index]) {
                printf("Error: Item %s (Shop: %s) is out of stock or invalid.\n", product_names[index], shop_names[index]);
                for (int i = 0; i < cart_count; i++) {
                    free(product_names[i]);
                    free(shop_names[i]);
                }
                free(product_names);
                free(shop_names);
                free(unit_prices);
                free(totals);
                free(stock_quantities);
                free(cart_items);
                fclose(cart_file);
                fclose(items_file);
                return 0;
            }
            index++;
        }
    }
    fclose(cart_file);
    fclose(items_file);
    float overall_total = 0.0;
    for (int i = 0; i < cart_count; i++) {
        overall_total += totals[i];
    }
    FILE *customer_file = fopen("../data/customer.txt", "r");
    if (!customer_file) {
        printf("Error accessing customer.txt.\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    Customer customer;
    int found_customer = 0;
    while (fgets(line, sizeof(line), customer_file)) {
        int parsed = sscanf(line, "%19[^,],%49[^,],%f",
                            customer.id, customer.name, &customer.balance);
        if (parsed == 3 && strcmp(customer.id, userID) == 0) {
            found_customer = 1;
            break;
        }
    }
    fclose(customer_file);
    if (!found_customer) {
        printf("Customer with ID '%s' not found in customer.txt.\n", userID);
        printf("Please ensure your user ID matches the ID in customer.txt (e.g., CM05-06-CUS8).\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    if (customer.balance < overall_total) {
        printf("Insufficient balance. You need $%.2f but have $%.2f.\n", overall_total, customer.balance);
        printf("Returning to customer dashboard...\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    printf("\n=== Checkout ===\n");
    printf("------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-5s | %-15s | %-10s | %-10s |\n",
           "NO", "PRODUCT NAME", "QTY", "SHOP NAME", "UNIT PRICE", "TOTAL");
    printf("------------------------------------------------------------------------------------\n");
    for (int i = 0; i < cart_count; i++) {
        printf("| %-5d | %-20s | %-5d | %-15s | $%-9.2f | $%-9.2f |\n",
               i + 1, product_names[i], cart_items[i].qty, shop_names[i], unit_prices[i], totals[i]);
    }
    printf("------------------------------------------------------------------------------------\n");
    printf("| %-63s | $%-9.2f |\n", "OVERALL TOTAL", overall_total);
    printf("------------------------------------------------------------------------------------\n");
    char input[10];
    int result = get_validated_input("Confirm checkout? (y/n): ", input, sizeof(input), NULL, NULL);
    if (result <= 0 || tolower(input[0]) != 'y') {
        printf("Checkout cancelled. Returning to customer dashboard...\n");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    customer_file = fopen("../data/customer.txt", "r");
    FILE *temp_customer_file = fopen("../data/customer_temp.txt", "w");
    if (!customer_file || !temp_customer_file) {
        perror("Error updating customer.txt");
        if (customer_file) fclose(customer_file);
        if (temp_customer_file) fclose(temp_customer_file);
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    while (fgets(line, sizeof(line), customer_file)) {
        Customer temp_customer;
        int parsed = sscanf(line, "%19[^,],%49[^,],%f",
                            temp_customer.id, temp_customer.name, &temp_customer.balance);
        if (parsed == 3 && strcmp(temp_customer.id, userID) == 0) {
            temp_customer.balance -= overall_total;
            fprintf(temp_customer_file, "%s,%s,%.2f\n",
                    temp_customer.id, temp_customer.name, temp_customer.balance);
        } else {
            fprintf(temp_customer_file, "%s", line);
        }
    }
    fclose(customer_file);
    fclose(temp_customer_file);
    if (remove("../data/customer.txt") != 0 || rename("../data/customer_temp.txt", "../data/customer.txt") != 0) {
        perror("Error updating customer.txt");
        remove("../data/customer_temp.txt");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    items_file = fopen("../data/items.txt", "r");
    FILE *temp_items_file = fopen("../data/items_temp.txt", "w");
    if (!items_file || !temp_items_file) {
        perror("Error updating items.txt");
        if (items_file) fclose(items_file);
        if (temp_items_file) fclose(temp_items_file);
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    while (fgets(line, sizeof(line), items_file)) {
        Item item;
        char temp_datetime[MAX_DATETIME_LEN] = "";
        int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                           item.productID, item.storeID, item.storeName, item.name,
                           &item.qty, &item.price, item.expir, item.category,
                           item.manufacturing, item.country, item.distributor, temp_datetime);
        if (parsed == 11 || parsed == 12) {
            for (int i = 0; i < cart_count; i++) {
                if (strcmp(item.productID, cart_items[i].productID) == 0 &&
                    strcmp(item.storeID, cart_items[i].shopID) == 0) {
                    item.qty -= cart_items[i].qty;
                    break;
                }
            }
            fprintf(temp_items_file, "%s,%s,%s,%s,%d,%.2f,%s,%s,%s,%s,%s",
                    item.productID, item.storeID, item.storeName, item.name,
                    item.qty, item.price, item.expir, item.category,
                    item.manufacturing, item.country, item.distributor);
            if (parsed == 12) {
                fprintf(temp_items_file, ",%s", temp_datetime);
            }
            fprintf(temp_items_file, "\n");
        } else {
            fprintf(temp_items_file, "%s", line);
        }
    }
    fclose(items_file);
    fclose(temp_items_file);
    if (remove("../data/items.txt") != 0 || rename("../data/items_temp.txt", "../data/items.txt") != 0) {
        perror("Error updating items.txt");
        remove("../data/items_temp.txt");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    generate_invoice(userID, customer.name, cart_items, product_names, shop_names, unit_prices, totals, cart_count, customer.balance - overall_total);
    printf("✓ Checkout completed successfully! Total: $%.2f\n", overall_total);
    for (int i = 0; i < cart_count; i++) {
        free(product_names[i]);
        free(shop_names[i]);
    }
    free(product_names);
    free(shop_names);
    free(unit_prices);
    free(totals);
    free(stock_quantities);
    free(cart_items);
    cart_file = fopen("../data/cart.txt", "r");
    FILE *temp_cart_file = fopen("../data/cart_temp.txt", "w");
    if (!cart_file || !temp_cart_file) {
        perror("Error updating cart.txt");
        if (cart_file) fclose(cart_file);
        if (temp_cart_file) fclose(temp_cart_file);
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    while (fgets(line, sizeof(line), cart_file)) {
        CartItem temp_cart_item;
        int parsed = sscanf(line, "%19[^,],%9[^,],%19[^,],%d,%17[^\n]",
                            temp_cart_item.userID, temp_cart_item.productID, temp_cart_item.shopID,
                            &temp_cart_item.qty, temp_cart_item.datetime);
        if (parsed != 5 || strcmp(temp_cart_item.userID, userID) != 0) {
            fprintf(temp_cart_file, "%s", line);
        }
    }
    fclose(cart_file);
    fclose(temp_cart_file);
    if (remove("../data/cart.txt") != 0 || rename("../data/cart_temp.txt", "../data/cart.txt") != 0) {
        perror("Error updating cart.txt");
        remove("../data/cart_temp.txt");
        for (int i = 0; i < cart_count; i++) {
            free(product_names[i]);
            free(shop_names[i]);
        }
        free(product_names);
        free(shop_names);
        free(unit_prices);
        free(totals);
        free(stock_quantities);
        free(cart_items);
        return 0;
    }
    printf("Returning to customer dashboard...\n");
    return 1;
}

#endif