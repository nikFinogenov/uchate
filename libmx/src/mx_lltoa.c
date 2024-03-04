#include "libmx.h"

char *mx_lltoa(long long n) {
    long long temp = n;
    int len = 0;
    char *str;

    if (n == 0) {
        str = (char *)malloc(2 * sizeof(char));
        mx_strcpy(str, "0");
        return str;
    }

    if (temp < 0) {
        len++;
        temp *= -1;
    }

    while (temp > 0) {
        len++;
        temp /= 10;
    }

    str = (char *)malloc((len + 1) * sizeof(char));

    if (n < 0) {
        str[0] = '-';
        n *= -1;
    }

    int i = len - 1;
    while (n > 0) {
        str[i] = n % 10 + '0';
        n /= 10;
        i--;
    }
    str[len] = '\0';

    return str;
}
