#include "../inc/libmx.h"

char *mx_strncat(char *restrict s1, const char *restrict s2, size_t n) {
    char *dest = s1;
    
    while (*s1)
        s1++;
    
    while (n-- && (*s1++ = *s2++))
        ;
    
    *s1 = '\0';
    
    return dest;
}
