#ifndef VIEWITEMS_H
#define VIEWITEMS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h" // Include struct definitions

// Comparison function for qsort to sort items by name (A to Z)
int compare_items(const void *a, const void *b) {
    return strcmp(((Item *)a)->name, ((Item *)b)->name);
}

// Function to view items for a specific shop
void view_items(const char *storeID, const char *storeName) {
    FILE *file = fopen("../data/items.txt", "r");
    if (file == NULL) {
        printf("No items found. The items.txt file does not exist or cannot be opened.\n");
        printf("Returning to shop dashboard...\n");
        return;
    }

    // Count items for this store to allocate memory
    int item_count = 0;
    char line[512];
    Item temp_item;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^\n]",
                   temp_item.productID, temp_item.storeID, temp_item.storeName, temp_item.name, &temp_item.qty, &temp_item.price,
                   temp_item.expir, temp_item.category, temp_item.manufacturing, temp_item.country, temp_item.distributor) == 11) {
            if (strcmp(temp_item.storeID, storeID) == 0 && strcmp(temp_item.storeName, storeName) == 0) {
                item_count++;
            }
        }
    }
    rewind(file);

    if (item_count == 0) {
        printf("No items found for shop '%s'.\n", storeName);
        fclose(file);
        printf("Returning to shop dashboard...\n");
        return;
    }

    // Allocate memory for items
    Item *items = (Item *)malloc(item_count * sizeof(Item));
    if (!items) {
        perror("Error allocating memory for items.");
        fclose(file);
        printf("Returning to shop dashboard...\n");
        return;
    }

    // Read items into the array
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^\n]",
                   temp_item.productID, temp_item.storeID, temp_item.storeName, temp_item.name, &temp_item.qty, &temp_item.price,
                   temp_item.expir, temp_item.category, temp_item.manufacturing, temp_item.country, temp_item.distributor) == 11) {
            if (strcmp(temp_item.storeID, storeID) == 0 && strcmp(temp_item.storeName, storeName) == 0) {
                items[index++] = temp_item;
            }
        }
    }
    fclose(file);

    // Sort items by name
    qsort(items, item_count, sizeof(Item), compare_items);

    // Display table header
    printf("\n=== ITEM LIST ===\n");
    printf("+================================================================== ITEM ===============================================================================+\n");
    printf("| %-5s | %-15s | %-20s | %-5s | %-8s | %-10s | %-12s | %-20s | %-7s | %-20s |\n",
           "NO", "ID", "NAME", "QTY", "PRICE", "EXPIR", "CATEGORY", "manufacturing", "Country", "Distributor(SHOP)");
    printf("+=======================================================================================================================================================+\n");

    // Display items
    for (int i = 0; i < item_count; i++) {
        printf("| %-5d | %-15s | %-20s | %-5d | $%-7.2f | %-10s | %-12s | %-20s | %-7s | %-20s |\n",
               i + 1, items[i].productID, items[i].name, items[i].qty, items[i].price,
               items[i].expir, items[i].category, items[i].manufacturing, items[i].country, items[i].distributor);
    }
    printf("+=======================================================================================================================================================+\n");

    // Free memory
    free(items);

    printf("Returning to shop dashboard...\n");
}

#endif