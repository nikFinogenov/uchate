#include "../inc/libmx.h"

int mx_get_substr_index(const char *str, const char *sub) {
	if (str == NULL || sub == NULL) {
        return -2;
    }

    const char *ptr = mx_strstr(str, sub);

    if (ptr != NULL) {
        return ptr - str; 
    } else {
        return -1;
    }
}
