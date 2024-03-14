#include "../inc/libmx.h"

char *mx_strndup(const char *s1, size_t n) {
	if (s1 == NULL) {
        return NULL;
    }

    size_t length = mx_strlen(s1);

    if (n < length) {
        length = n;
    }

    char *duplicate = mx_strnew(length);

    if (duplicate != NULL) {
        mx_strncpy(duplicate, s1, length);
        duplicate[length] = '\0';
    }

    return duplicate;
}
