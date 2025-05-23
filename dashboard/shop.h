#ifndef SHOP_H
#define SHOP_H

#include <stdio.h>
#include <string.h>


int shopMenu(){
    printf("\n=== SHOP DASHBOARD ===\n");
    printf("1. Edit shop name\n");
    printf("2. Add item\n");
    printf("3. Update stock\n");
    printf("4. View items\n");
    printf("5. Back\n");
};

int shop_dashboard() {
    return shopMenu();
}

#endif