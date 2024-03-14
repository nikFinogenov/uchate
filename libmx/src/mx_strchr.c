#include "../inc/libmx.h"

char* mx_strchr(const char* str, int c) {
    int i = 0;
    while (str[i] && str[i] != c) ++i;
    return c == str[i] ? (char*)str + i : NULL;
}
