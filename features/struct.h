#ifndef STRUCT_H
#define STRUCT_H

#define MAX_ID_LEN 20
#define MAX_PRODUCT_ID_LEN 10 // For Product ID (e.g., PRD001)
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_SHOP_NAME_LEN 100
#define MAX_ITEM_NAME_LEN 50
#define MAX_EXPIRY_LEN 11
#define MAX_CATEGORY_LEN 50
#define MAX_MANUFACTURING_LEN 50
#define MAX_COUNTRY_LEN 50
#define MAX_DISTRIBUTOR_LEN 100

// Struct for userName
typedef struct {
    char id[MAX_ID_LEN];           // UniqueID (e.g., SPMtM290525, CM290525ali7, AD1)
    char username[MAX_USERNAME_LEN]; // Username
    char password[MAX_PASSWORD_LEN]; // Encrypted password
    int usertype;                   // 0=ADMIN, 1=SHOP, 2=CUSTOMER
} userName;

// Struct for Shop
typedef struct {
    char id[MAX_ID_LEN];           // Inherited from userName.id for usertype=1
    char shop_name[MAX_SHOP_NAME_LEN]; // Shop name
} Shop;

// Struct for Customer
typedef struct {
    char id[MAX_ID_LEN];           // Matches userName.id for usertype=2
    char name[MAX_USERNAME_LEN];   // Matches userName.username
    float balance;                 
} Customer;

// Struct for Item
typedef struct {
    char productID[MAX_PRODUCT_ID_LEN]; // Unique Product ID (e.g., PRD001)
    char storeID[MAX_ID_LEN];
    char storeName[MAX_SHOP_NAME_LEN];
    char name[MAX_ITEM_NAME_LEN];
    int qty;
    float price;
    char expir[MAX_EXPIRY_LEN];
    char category[MAX_CATEGORY_LEN]; // Restricted to "Grocery" or "Electronic"
    char manufacturing[MAX_MANUFACTURING_LEN];
    char country[MAX_COUNTRY_LEN];  // Restricted to "USA", "CHINA", "KOREA", "KHMER"
    char distributor[MAX_DISTRIBUTOR_LEN];
} Item;

// Helper function to validate numeric input (for single-digit choices like 1 or 2)
int is_numeric(const char *str) {
    if (strlen(str) == 0) return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

#endif