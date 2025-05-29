#ifndef STRUCT_H
#define STRUCT_H

#define MAX_ID_LEN 20
#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_SHOP_NAME_LEN 100

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

typedef struct {
    char id[MAX_ID_LEN];           // Matches userName.id for usertype=2
    char name[MAX_USERNAME_LEN];   // Matches userName.username
    float balance;                 // Default 3000.00
} Customer;

#endif