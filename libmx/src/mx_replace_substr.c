#include "../inc/libmx.h"

char *mx_replace_substr(const char *str, const char *sub, const char *replace) {
	if (str == NULL || sub == NULL || replace == NULL) {
        return NULL;
    }

    int str_len = mx_strlen(str);
    int sub_len = mx_strlen(sub);

    if (str_len == 0 || sub_len == 0) {
        return mx_strdup(str);
    }

    char *result = mx_strnew(0);

    int i = 0;
    while (i < str_len) {
        if (mx_strncmp(&str[i], sub, sub_len) == 0) {
            result = mx_strcat(result, replace);
            i += sub_len;
        } else {
            char tmp[2] = {str[i], '\0'};
            result = mx_strcat(result, tmp);
            i++;
        }
    }

    return result;
}
