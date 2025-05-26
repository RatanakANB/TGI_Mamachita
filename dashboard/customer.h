#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <stdio.h>
#include <string.h>

int customerMenu() {
    printf("\n=== CUSTOMER DASHBOARD ===\n");
    printf("1. Browse shops\n");
    printf("2. View cart\n");
    printf("3. Add funds\n");
    printf("4. Checkout\n");
    printf("5. Back\n");
    return 0;
}

int customer_dashboard() {


    return customerMenu();
}


#endif