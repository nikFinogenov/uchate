#include "../inc/libmx.h"

char *mx_strjoin(const char *s1, const char *s2) {
	if (s1 == NULL && s2 == NULL) {
        return NULL;
    }

    size_t s1_len = (s1 != NULL) ? mx_strlen(s1) : 0;
    size_t s2_len = (s2 != NULL) ? mx_strlen(s2) : 0;

    size_t total_len = s1_len + s2_len;

    char *result = mx_strnew(total_len);

    if (result == NULL) {
        return NULL;
    }

    if (s1 != NULL) {
        mx_strcpy(result, s1);
    }
    
    if (s2 != NULL) {
        mx_strcat(result, s2);
    }

    return result;
}
