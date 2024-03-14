#include "../inc/libmx.h"

char *mx_strtok(char *str, const char *delim) {
    static char *token;
    if (str != NULL)
        token = str;

    if (token == NULL)
        return NULL;

    char *start = token;
    while (*token != '\0' && !mx_strchr(delim, *token)) {
        token++;
    }

    if (*token == '\0') {
        token = NULL;
    } else {
        *token = '\0';
        token++;
    }

    return start;
}
