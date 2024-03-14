#include "../inc/libmx.h"

char* mx_char_to_str(const char source[]) {
    size_t length = mx_strlen(source);

    char* destination = (char*)malloc((length + 1) * sizeof(char));

    if (destination == NULL) {
        exit(1);
    }

    mx_strcpy(destination, source);

    return destination;
}
