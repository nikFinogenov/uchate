#include "../inc/libmx.h"

int mx_count_words(const char *str, char c) {
	if (str == NULL) {
        return -1;
    }

    int wordCount = 0;
    int inWord = 0;

    for (const char *ptr = str; *ptr != '\0'; ptr++) {
        if (*ptr == c) {
            inWord = 0;
        } else if (!inWord) {
            inWord = 1;
            wordCount++;
        }
    }

    return wordCount;
}
