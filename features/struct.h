#ifndef STRUCT_H
#define STRUCT_H

#define MAX_ID_LEN 20
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_SHOP_NAME_LEN 100
#define MAX_ITEM_NAME_LEN 50
#define MAX_PRODUCT_ID_LEN 10
#define MAX_DATETIME_LEN 18
#define MAX_DISTRIBUTOR_LEN 100
#define MAX_EXPIRY_LEN 11
#define MAX_MANUFACTURING_LEN 50

typedef struct {
    char id[MAX_ID_LEN];
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int usertype;
    char creation_datetime[MAX_DATETIME_LEN];
} userName;

typedef struct {
    char id[MAX_ID_LEN];
    char shop_name[MAX_SHOP_NAME_LEN];
} Shop;

typedef struct {
    char id[MAX_ID_LEN];
    char name[MAX_USERNAME_LEN];
    float balance;
} Customer;

typedef struct {
    char productID[MAX_PRODUCT_ID_LEN];
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
    char created_datetime[MAX_DATETIME_LEN];
} Item;

typedef struct {
    char userID[MAX_ID_LEN];
    char productID[MAX_PRODUCT_ID_LEN];
    char shopID[MAX_ID_LEN];
    int qty;
    char datetime[MAX_DATETIME_LEN];
} CartItem;

#endif