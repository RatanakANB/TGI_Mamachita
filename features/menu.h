#ifndef MENU_H
#define MENU_H
#include <stdio.h>

int menu();

int menu() {
    int choice;
    printf("\n1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n");
    printf("Please select an option: ");
    scanf("%d", &choice);
    while (getchar() != '\n'); // Clear input buffer
    return choice;
}

#endif