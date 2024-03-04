#include "../inc/libmx.h"

void mx_str_reverse(char *s) {
	if (!s)
        return;

    int len = mx_strlen(s);
    if (len <= 1)
        return;

    char *start = s;
    char *end = s + len - 1;

    while (start < end) {
        mx_swap_char(start, end);
        start++;
        end--;
    }
}
