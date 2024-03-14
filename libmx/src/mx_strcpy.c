#include "../inc/libmx.h"

char *mx_strcpy(char *dst, const char *src) {
    char *pointer = dst;
    while (*src != '\0') {
        *dst = *src;
        dst++;
        src++;
    }
    *dst = '\0';
    return pointer;
}
