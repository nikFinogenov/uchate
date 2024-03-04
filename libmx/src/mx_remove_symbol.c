#include "../inc/libmx.h"

void mx_remove_symbol(char *str, char symbol) {
    if (str == NULL) {
        return;
    }

    int readIndex = 0;
    int writeIndex = 0;

    while (str[readIndex] != '\0') {
        if (str[readIndex] != symbol) {
            str[writeIndex] = str[readIndex];
            writeIndex++;
        }

        readIndex++;
    }

    str[writeIndex] = '\0';
}
