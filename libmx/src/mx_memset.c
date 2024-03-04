#include "../inc/libmx.h"

void *mx_memset(void *b, int c, size_t len) {
	unsigned char *ptr = (unsigned char *)b;
    unsigned char value = (unsigned char)c;

    for (size_t i = 0; i < len; i++) {
        ptr[i] = value;
    }

    return b;
}
