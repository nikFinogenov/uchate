#include "../inc/libmx.h"

void mx_printint(int n) {
    int length = 0;
    int b_len = 20;
    char buffer[b_len];
    int is_negative = 0;

    if (n == 0) {
        mx_printchar('0');
        return;
    }

    if (n < 0) {
        is_negative = 1;
        if (n == -2147483648) {
            buffer[length++] = '8';
            n = 214748364;  
        } else {
            n = -n;
        }
    }

    while (n > 0) {
        buffer[length++] = '0' + (n % 10);
        n /= 10;
    }

    if (is_negative == 1) {
        buffer[length++] = '-';
    }

    for (int i = 0, j = length - 1; i < j; i++, j--) {
        char temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    for (int i = 0; i < length; i++) {
        mx_printchar(buffer[i]);
    }
}
