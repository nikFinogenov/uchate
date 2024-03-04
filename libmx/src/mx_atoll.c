#include "../inc/libmx.h"

long long mx_atoll(const char *str) {
    long long result = 0;
    bool negative = false;

    while (*str != '\0') {
        if (*str == '-') {
            negative = true;
        } else if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        } else {
            break;
        }

        str++;
    }

    if (negative) {
        result = -result;
    }

    return result;
}
