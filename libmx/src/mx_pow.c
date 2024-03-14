#include "../inc/libmx.h"

double mx_pow(double n, unsigned int pow) {
    double result = n;
    if (pow == 0) {
        result = 1;
    }

    for (unsigned int i = 1; i < pow; i++) {
        result *= n;
    }

    return result;
}
