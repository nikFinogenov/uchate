#include "../inc/libmx.h"

void *mx_memmove(void *dst, const void *src, size_t len) {
	unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;

    if (d < s) {
        for (size_t i = 0; i < len; i++) {
            d[i] = s[i];
        }
    } else if (d > s) {
        for (size_t i = len; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }

    return dst;
	
}
