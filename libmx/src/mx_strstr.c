#include "../inc/libmx.h"


char *mx_strstr(const char *haystack, const char *needle) {
	if (haystack == NULL || needle == NULL)
        return NULL;

    int haystack_len = mx_strlen(haystack);
    int needle_len = mx_strlen(needle);

    for (int i = 0; i <= haystack_len - needle_len; i++) {
        int j;
        for (j = 0; j < needle_len; j++) {
            if (haystack[i + j] != needle[j])
                break;
        }
        if (j == needle_len)
            return (char *)(haystack + i);
    }

    return NULL;
}
