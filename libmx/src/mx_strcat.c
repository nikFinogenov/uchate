#include "../inc/libmx.h"

char *mx_strcat(char *restrict s1, const char *restrict s2) {
	if (s1 == NULL || s2 == NULL) {
        return s1; 
    }

    char *originalS1 = s1; 

    while (*s1 != '\0') {
        s1++;
    }

    while (*s2 != '\0') {
        *s1 = *s2;
        s1++;
        s2++;
    }

    *s1 = '\0';

    return originalS1;
}
