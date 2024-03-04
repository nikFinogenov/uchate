#include "../inc/libmx.h"

int mx_memcmp(const void *s1, const void *s2, size_t n) {
	const unsigned char *ptr1 = (const unsigned char *)s1;
    const unsigned char *ptr2 = (const unsigned char *)s2;

    for (size_t i = 0; i < n; i++) {
        if (ptr1[i] != ptr2[i]) {
            return (int)(ptr1[i] - ptr2[i]);
        }
    }

    return 0;
}
