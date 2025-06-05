#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "struct.h"

#define MAX_RETRIES 3

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void trim_whitespace(char *str) {
    if (!str) return;
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

int is_valid_float(const char *str) {
    if (!str || *str == '\0') return 0;
    int has_decimal = 0;
    int i = 0;
    if (str[i] == '-') i++;
    for (; str[i]; i++) {
        if (str[i] == '.') {
            if (has_decimal) return 0;
            has_decimal = 1;
            continue;
        }
        if (!isdigit(str[i])) return 0;
    }
    if (i == 0 || (i == 1 && str[0] == '-')) return 0;
    float value = atof(str);
    if (value < 0) return 0;
    return 1;
}

int get_validated_input(const char *prompt, char *buffer, size_t size, 
                       int (*validator)(const char *), const char *error_msg) {
    int attempts = 0;
    while (attempts < MAX_RETRIES) {
        printf("%s", prompt);
        if (fgets(buffer, size, stdin)) {
            buffer[strcspn(buffer, "\n")] = '\0';
            trim_whitespace(buffer);
            if (strcmp(buffer, "back") == 0) {
                return -1;
            }
            if (validator && !validator(buffer)) {
                printf("%s\n", error_msg);
                attempts++;
                continue;
            }
            if (strlen(buffer) > 0) {
                return 1;
            }
        }
        printf("Input cannot be empty. Please try again.\n");
        attempts++;
    }
    printf("Too many invalid attempts. Returning to shop dashboard...\n");
    return 0;
}

int is_item_exists(const char *storeID, const char *item_name) {
    if (!storeID || !item_name || strlen(item_name) == 0) return 0;
    FILE *file = fopen("../data/items.txt", "r");
    if (!file) return 0;
    char line[512];
    Item item;
    int found = 0;
    while (fgets(line, sizeof(line), file) && !found) {
        char temp_datetime[MAX_DATETIME_LEN] = "";
        int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                            item.productID, item.storeID, item.storeName, item.name, 
                            &item.qty, &item.price, item.expir, item.category, 
                            item.manufacturing, item.country, item.distributor, temp_datetime);
        if (parsed == 11) temp_datetime[0] = '\0'; // Old format without datetime
        if ((parsed == 11 || parsed == 12) && strcmp(item.storeID, storeID) == 0 && 
            strcasecmp(item.name, item_name) == 0) {
            found = 1;
        }
    }
    fclose(file);
    return found;
}

int is_product_id_exists(const char *storeID, const char *product_id) {
    if (!storeID || !product_id || strlen(product_id) == 0) return 0;
    FILE *file = fopen("../data/items.txt", "r");
    if (!file) return 0;
    char line[512];
    Item item;
    int found = 0;
    while (fgets(line, sizeof(line), file) && !found) {
        char temp_datetime[MAX_DATETIME_LEN] = "";
        int parsed = sscanf(line, "%9[^,],%19[^,],%99[^,],%49[^,],%d,%f,%10[^,],%49[^,],%49[^,],%49[^,],%99[^,],%17[^\n]",
                            item.productID, item.storeID, item.storeName, 
                            item.name, &item.qty, &item.price, item.expir, 
                            item.category, item.manufacturing, item.country, 
                            item.distributor, temp_datetime);
        if (parsed == 11) temp_datetime[0] = '\0'; // Old format without datetime
        if ((parsed == 11 || parsed == 12) && strcmp(item.storeID, storeID) == 0 && 
            strcmp(item.productID, product_id) == 0) {
            found = 1;
        }
    }
    fclose(file);
    return found;
}

void get_current_datetime(char *datetime) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(datetime, MAX_DATETIME_LEN, "%d-%m-%y %H:%M:%S", tm);
}

#endif