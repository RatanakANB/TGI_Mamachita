#ifndef INVOICE_H
#define INVOICE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"
#include "Utils.h"

typedef struct {
    char invoice_id[13];
    char user_id[MAX_ID_LEN];
    float total_spend;
    char datetime[MAX_DATETIME_LEN];
} InvoiceSummary;

int generate_invoice_id(char *invoice_id, size_t size) {
    char datetime[MAX_DATETIME_LEN];
    get_current_datetime(datetime);
    char date_prefix[7];
    strncpy(date_prefix, datetime + 2, 6); // mmddyy
    date_prefix[6] = '\0';

    FILE *invoice_file = fopen("../data/invoices.txt", "r");
    int max_counter = 0;
    if (invoice_file) {
        char line[256];
        while (fgets(line, sizeof(line), invoice_file)) {
            char temp_invoice_id[13];
            int parsed = sscanf(line, "%12[^,]", temp_invoice_id);
            if (parsed == 1 && strncmp(temp_invoice_id, "INV", 3) == 0 && strlen(temp_invoice_id) == 12) {
                if (strncmp(temp_invoice_id + 3, date_prefix, 6) == 0) {
                    int counter;
                    if (sscanf(temp_invoice_id + 9, "%4d", &counter) == 1) {
                        if (counter > max_counter) {
                            max_counter = counter;
                        }
                    }
                }
            }
        }
        fclose(invoice_file);
    }

    if (max_counter >= 9999) {
        printf("Error: Maximum invoice count reached for today.\n");
        return 0;
    }

    snprintf(invoice_id, size, "INV%s%04d", date_prefix, max_counter + 1);
    return 1;
}

int generate_invoice(const char *userID, const char *customer_name, CartItem *cart_items, 
                    char **product_names, char **shop_names, float *unit_prices, 
                    float *totals, int cart_count, float remaining_balance) {
    char invoice_id[13];
    if (!generate_invoice_id(invoice_id, sizeof(invoice_id))) {
        return 0;
    }

    char datetime[MAX_DATETIME_LEN];
    get_current_datetime(datetime);

    FILE *invoice_file = fopen("../data/invoices.txt", "a");
    if (!invoice_file) {
        printf("Error: Cannot open invoices.txt for writing.\n");
        return 0;
    }

    float overall_total = 0.0;
    for (int i = 0; i < cart_count; i++) {
        overall_total += totals[i];
        fprintf(invoice_file, "%s,%s,%s,%s,%d,%.2f,%.2f,%s\n",
                invoice_id, userID, cart_items[i].productID, cart_items[i].shopID,
                cart_items[i].qty, unit_prices[i], totals[i], datetime);
    }
    fclose(invoice_file);

    printf("\n=== INVOICE ===\n");
    printf("Invoice ID: %s\n", invoice_id);
    printf("Customer: %s\n", customer_name);
    printf("Date: %s\n", datetime);
    printf("-----------------------------------------------------\n");
    printf("| %-20s | %-15s | %-5s | %-10s | %-10s |\n", 
           "PRODUCT NAME", "SHOP NAME", "QTY", "UNIT PRICE", "TOTAL");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < cart_count; i++) {
        printf("| %-20s | %-15s | %-5d | $%-9.2f | $%-9.2f |\n",
               product_names[i], shop_names[i], cart_items[i].qty, unit_prices[i], totals[i]);
    }
    printf("-----------------------------------------------------\n");
    printf("| %-20s | %-15s | %-5s | %-10s | $%-9.2f |\n", 
           "OVERALL TOTAL", "", "", "", overall_total);
    printf("-----------------------------------------------------\n");
    printf("Remaining Balance: $%.2f\n", remaining_balance);
    return 1;
}

int retrieve_invoice(const char *invoice_id) {
    if (!invoice_id || strlen(invoice_id) != 12 || strncmp(invoice_id, "INV", 3) != 0) {
        printf("Invalid invoice ID format. Expected format: INVmmddyyXXXX (e.g., INV0606250001).\n");
        return 0;
    }
    FILE *invoice_file = fopen("../data/invoices.txt", "r");
    if (!invoice_file) {
        printf("No invoices found. The invoices.txt file does not exist or cannot be opened.\n");
        return 0;
    }
    char line[256];
    int found = 0;
    char customer_id[MAX_ID_LEN] = "";
    char datetime[MAX_DATETIME_LEN] = "";
    float overall_total = 0.0;
    printf("\n=== INVOICE ===\n");
    printf("Invoice ID: %s\n", invoice_id);
    
    while (fgets(line, sizeof(line), invoice_file)) {
        char temp_invoice_id[13], user_id[MAX_ID_LEN], product_id[MAX_PRODUCT_ID_LEN], shop_id[MAX_ID_LEN];
        int qty;
        float unit_price, total;
        char temp_datetime[MAX_DATETIME_LEN];
        int parsed = sscanf(line, "%12[^,],%19[^,],%9[^,],%19[^,],%d,%f,%f,%17[^\n]",
                            temp_invoice_id, user_id, product_id, shop_id,
                            &qty, &unit_price, &total, temp_datetime);
        if (parsed == 8 && strcmp(temp_invoice_id, invoice_id) == 0) {
            strcpy(customer_id, user_id);
            strcpy(datetime, temp_datetime);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Invoice '%s' not found.\n", invoice_id);
        fclose(invoice_file);
        return 0;
    }
    
    FILE *customer_file = fopen("../data/customer.txt", "r");
    char customer_name[MAX_USERNAME_LEN] = "Unknown";
    if (customer_file) {
        char customer_line[256];
        while (fgets(customer_line, sizeof(customer_line), customer_file)) {
            char temp_id[MAX_ID_LEN], temp_name[MAX_USERNAME_LEN];
            float balance;
            int parsed = sscanf(customer_line, "%19[^,],%49[^,],%f",
                               temp_id, temp_name, &balance);
            if (parsed == 3 && strcmp(temp_id, customer_id) == 0) {
                strcpy(customer_name, temp_name);
                break;
            }
        }
        fclose(customer_file);
    }
    
    printf("Customer: %s\n", customer_name);
    printf("Date: %s\n", datetime);
    printf("-----------------------------------------------------\n");
    printf("| %-20s | %-15s | %-5s | %-10s | %-10s |\n", 
           "PRODUCT NAME", "SHOP NAME", "QTY", "UNIT PRICE", "TOTAL");
    printf("-----------------------------------------------------\n");
    
    rewind(invoice_file);
    while (fgets(line, sizeof(line), invoice_file)) {
        char temp_invoice_id[13], user_id[MAX_ID_LEN], product_id[MAX_PRODUCT_ID_LEN], shop_id[MAX_ID_LEN];
        int qty;
        float unit_price, total;
        char temp_datetime[MAX_DATETIME_LEN];
        int parsed = sscanf(line, "%12[^,],%19[^,],%9[^,],%19[^,],%d,%f,%f,%17[^\n]",
                            temp_invoice_id, user_id, product_id, shop_id,
                            &qty, &unit_price, &total, temp_datetime);
        if (parsed == 8 && strcmp(temp_invoice_id, invoice_id) == 0) {
            char product_name[MAX_ITEM_NAME_LEN] = "Unknown";
            char shop_name[MAX_SHOP_NAME_LEN] = "Unknown";
            FILE *items_file = fopen("../data/items.txt", "r");
            if (items_file) {
                char item_line[512];
                while (fgets(item_line, sizeof(item_line), items_file)) {
                    Item item;
                    char temp_datetime_item[MAX_DATETIME_LEN] = "";
                    int item_parsed = sscanf(item_line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                                            item.productID, item.storeID, item.storeName, item.name,
                                            &item.qty, &item.price, item.expir, item.category,
                                            item.manufacturing, item.country, item.distributor, temp_datetime_item);
                    if ((item_parsed == 11 || item_parsed == 12) && 
                        strcmp(item.productID, product_id) == 0 && 
                        strcmp(item.storeID, shop_id) == 0) {
                        strcpy(product_name, item.name);
                        strcpy(shop_name, item.storeName);
                        break;
                    }
                }
                fclose(items_file);
            }
            printf("| %-20s | %-15s | %-5d | $%-9.2f | $%-9.2f |\n",
                   product_name, shop_name, qty, unit_price, total);
            overall_total += total;
        }
    }
    fclose(invoice_file);
    printf("-----------------------------------------------------\n");
    printf("| %-20s | %-15s | %-5s | %-10s | $%-9.2f |\n", 
           "OVERALL TOTAL", "", "", "", overall_total);
    printf("-----------------------------------------------------\n");
    return 1;
}

int list_invoices(const char *userID) {
    if (!userID || strlen(userID) < 10 || strncmp(userID, "CM", 2) != 0) {
        printf("Invalid user ID format. Expected format: CMdd-mm-CUSX (e.g., CM05-06-CUS8).\n");
        return 0;
    }

    FILE *invoice_file = fopen("../data/invoices.txt", "r");
    if (!invoice_file) {
        printf("Error opening invoices.txt.\n");
        return 0;
    }

    char line[512];
    int invoice_count = 0;
    InvoiceSummary invoices[100];
    char processed_invoices[100][13];
    int processed_count = 0;

    while (fgets(line, sizeof(line), invoice_file)) {
        char temp_invoice_id[13], temp_user_id[MAX_ID_LEN], product_id[MAX_PRODUCT_ID_LEN], shop_id[MAX_ID_LEN];
        int qty;
        float unit_price, total;
        char datetime[MAX_DATETIME_LEN];
        int parsed = sscanf(line, "%12[^,],%19[^,],%9[^,],%19[^,],%d,%f,%f,%17[^\n]",
                            temp_invoice_id, temp_user_id, product_id, shop_id,
                            &qty, &unit_price, &total, datetime);
        if (parsed == 8 && strcmp(temp_user_id, userID) == 0 && strncmp(temp_invoice_id, "INV", 3) == 0 && strlen(temp_invoice_id) == 12) {
            int found = 0;
            for (int i = 0; i < processed_count; i++) {
                if (strcmp(processed_invoices[i], temp_invoice_id) == 0) {
                    found = 1;
                    for (int j = 0; j < invoice_count; j++) {
                        if (strcmp(invoices[j].invoice_id, temp_invoice_id) == 0) {
                            invoices[j].total_spend += total;
                            break;
                        }
                    }
                    break;
                }
            }
            if (!found) {
                strcpy(invoices[invoice_count].invoice_id, temp_invoice_id);
                strcpy(invoices[invoice_count].user_id, temp_user_id);
                invoices[invoice_count].total_spend = total;
                strcpy(invoices[invoice_count].datetime, datetime);
                strcpy(processed_invoices[processed_count], temp_invoice_id);
                invoice_count++;
                processed_count++;
                if (invoice_count >= 100) { break; }
            }
        }
    }
    fclose(invoice_file);

    if (invoice_count == 0) {
        printf("No invoices found for user %s\n", userID);
        printf("Returning to customer dashboard...\n");
        return 0;
    }

    printf("\n=== Summary of Invoices ===\n");
    printf("-----------------------------------------------------\n");
    printf("| %-5s | %-12s | %-12s | %-17s |\n",
           "NO.", "INVOICE ID", "TOTAL SPEND", "DATETIME");
    for (int i = 0; i < invoice_count; i++) {
        printf("| %-5d | %-12s | $%-11.2f | %-17s |\n",
               i + 1, invoices[i].invoice_id, invoices[i].total_spend, invoices[i].datetime);
    }
    printf("-----------------------------------------------------\n");

    char input[100];
    printf("Enter invoice number or ID (or 'back' to return): ");
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = '\0';
        trim_whitespace(input);
        if (strcmp(input, "back") == 0) {
            printf("Returning to customer dashboard...\n");
            return 0;
        }
        if (strlen(input) == 12 && strncmp(input, "INV", 3) == 0) {
            for (int i = 0; i < invoice_count; i++) {
                if (strcmp(invoices[i].invoice_id, input) == 0) {
                    return retrieve_invoice(input);
                }
            }
            printf("Invalid invoice ID '%s'. Please enter a valid invoice ID or number.\n", input);
            printf("Returning to customer dashboard...\n");
            return 0;
        }
        if (is_numeric(input)) {
            int invoice_index = atoi(input) - 1;
            if (invoice_index >= 0 && invoice_index < invoice_count) {
                return retrieve_invoice(invoices[invoice_index].invoice_id);
            }
            printf("Invalid invoice number. Please select a number between 1 and %d.\n", invoice_count);
            printf("Returning to customer dashboard...\n");
            return 0;
        }
        printf("Invalid input. Please enter a number or a valid invoice ID (e.g., INV0606250001).\n");
        printf("Returning to customer dashboard...\n");
        return 0;
    }
    printf("Error reading input. Returning to customer dashboard...\n");
    return 0;
}

#endif