#include <libmx.h>

int mx_get_length(char **arr) {
    int length = 0;
    while (arr[length] != NULL) {
        length++;
    }

    return length;
}
