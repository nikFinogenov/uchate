#include "../inc/libmx.h"

char **mx_strsplit(const char *s, char c) {
	if (s == NULL) {
        return NULL;
    }

    int word_count = mx_count_words(s, c);

    char **arr = (char **)malloc((word_count + 1) * 8);
    
    if (arr == NULL) {
        return NULL;
    }

    int word_index = 0;
    bool in_word = false;
    size_t word_start = 0;

    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == c) {
            if (in_word) {
                size_t word_length = i - word_start;
                arr[word_index] = mx_strndup(&s[word_start], word_length);
                word_index++;
                in_word = false;
            }
        } else if (!in_word) {
            word_start = i;
            in_word = true;
        }
    }

    if (in_word) {
        size_t word_length = mx_strlen(&s[word_start]);
        arr[word_index] = mx_strndup(&s[word_start], word_length);
    }

    arr[word_count] = NULL;

    return arr;
}
