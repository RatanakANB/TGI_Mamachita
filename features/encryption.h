#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <ctype.h>

static inline void encrypt_password(char *password) {
    for (int i = 0; password[i] != '\0'; i++) {
        if (isdigit(password[i])) {
            int num = password[i] - '0';
            num = (num + 6) % 10;
            password[i] = num + '0';
        } else if (islower(password[i])) {
            password[i] = (password[i] - 'a' + 4) % 26 + 'a';
        } else if (isupper(password[i])) {
            password[i] = (password[i] - 'A' + 4) % 26 + 'A';
        }
    }
}

#endif