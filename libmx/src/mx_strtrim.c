#include "../inc/libmx.h"

char *mx_strtrim(const char *str) {
	if (str == NULL) {
        return NULL;
    }

    const char *start = str;
    while (*start != '\0' && mx_isspace(*start)) {
        start++;
    }

    const char *end = str;
    const char *last_non_space = NULL;
    while (*end != '\0') {
        if (!mx_isspace(*end)) {
            last_non_space = end;
        }
        end++;
    }

    size_t trimmed_length = 0;
    if (last_non_space != NULL) {
        trimmed_length = last_non_space - start + 1;
    }

    char *trimmed_str = mx_strnew(trimmed_length);
    if (trimmed_str == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < trimmed_length; i++) {
        trimmed_str[i] = start[i];
    }

    return trimmed_str;
}
