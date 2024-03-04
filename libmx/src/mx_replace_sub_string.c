#include "../inc/libmx.h"

char* mx_replace_sub_string(char* original, const char* substring, const char* replacement) {
    char* result;
    char* ins;
    char* tmp;
    size_t len_substring = mx_strlen(substring);
    size_t len_replacement = mx_strlen(replacement);
    size_t len_original = mx_strlen(original);
    int count;

    if (len_substring == 0) {
        return mx_strdup(original);
    }

    ins = original;
    for (count = 0; (tmp = mx_strstr(ins, substring)); ++count) {
        ins = tmp + len_substring;
    }

    result = (char*)malloc(len_original + (len_replacement - len_substring) * count + 1);

    if (result == NULL) {
        exit(EXIT_FAILURE);
    }

    ins = original;
    tmp = result;

    while (count--) {
        char* after = mx_strstr(ins, substring);
        size_t segment_len = after - ins;
        tmp = mx_strncpy(tmp, ins, segment_len) + segment_len;
        tmp = mx_strcpy(tmp, replacement) + len_replacement;
        ins = after + len_substring;
    }

    mx_strcpy(tmp, ins);

    return result;
}
