#include "../inc/libmx.h"

int mx_strlen(const char *s) {
    int counter = 0;

    while(*s != '\0') {
        counter++;
        s++;
    }

    return counter;
}
