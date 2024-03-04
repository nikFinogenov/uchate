#include "libmx.h"

char* mx_ultoa(unsigned long value) {
    char* result = NULL;
    int digits = 0;
    unsigned long temp = value;

    do {
        digits++;
        temp /= 10;
    } while (temp > 0);

    result = (char*)malloc(digits + 1); 

    if (result == NULL) {
        perror("malloc");
        exit(1);
    }

    int index = digits - 1;
    do {
        result[index--] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    result[digits] = '\0'; 

    return result;
}
