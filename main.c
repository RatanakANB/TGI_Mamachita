#include <stdio.h>
#include "features/menu.h"
#include "features/login.h"
#include "features/register.h"

int main() {

    int option;
    printf("\n\t\t  Welcome to TGI Mamachita!\n");
    printf("\tThe most trusted E-Commerce platform you ever know.\n");
    while (1) {
        option = menu();
        printf("You selected option %d\n", option);
        if (option == 1) {
            login();
        } else if (option == 2) {
            register_user();
            break;
        } else if (option == 3) {
            printf("Exiting the program.\n");
            break;
        } else {
            printf("Invalid option. Please try again.\n");
        }
    }
    return 0;
}