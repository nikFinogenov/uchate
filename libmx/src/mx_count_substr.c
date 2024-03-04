#include "../inc/libmx.h"

int mx_count_substr(const char *str, const char *sub) {
	if (str == NULL || sub == NULL) {
        return -1;
    }

    if (*sub == '\0') {
        return 0;
    }

    int count = 0;
    const char *ptr = str;

    while ((ptr = mx_strstr(ptr, sub)) != NULL) {
        count++;
        ptr += mx_strlen(sub);
    }

    return count;
}
