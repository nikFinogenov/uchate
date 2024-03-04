#include "../inc/libmx.h"

void *mx_memchr(const void *s, int c, size_t n) {
	const unsigned char *ptr = (const unsigned char *)s;

    for (size_t i = 0; i < n; i++) {
        if (ptr[i] == (unsigned char)c) {
            return (void *)&ptr[i];
        }
    }

    return NULL;
}
