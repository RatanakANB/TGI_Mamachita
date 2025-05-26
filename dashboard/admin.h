#ifndef ADMIN_H
#define ADMIN_H

#include <stdio.h>
#include <string.h>


int adminMenu() {
    printf("\n\n=== ADMIN DASHBOARD ===\n\n");
    printf("1. List all shops\n");
    printf("2. Delete shop\n");
    printf("3. List all customers\n");
    printf("4. Back to main menu\n"); 
    return 0;
}

int admin_dashboard() {

    return adminMenu();
}

#endif