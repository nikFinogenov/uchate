#include "../inc/libmx.h"

char* mx_itoa(int number) {
    if (number == -2147483648) {
        return "-2147483648";
    }
    
    int max_digits = 1;
    int temp = number;
    while (temp / 10 != 0) {
        max_digits++;
        temp /= 10;
    }

    char* result = (char*)malloc((max_digits + 1) * sizeof(char));
    if (result == NULL) {
        return NULL;
    }

    int i = 0;
    if (number < 0) {
        result[i++] = '-';
        number = -number;
    }

    do {
        result[i++] = '0' + (number % 10);
        number /= 10;
    } while (number > 0);

    result[i] = '\0';

    int start = (result[0] == '-') ? 1 : 0;
    int end = i - 1;
    while (start < end) {
        char temp = result[start];
        result[start] = result[end];
        result[end] = temp;
        start++;
        end--;
    }

    return result;
}
