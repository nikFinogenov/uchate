#include "libmx.h"

bool mx_is_in_array(unsigned int *arr, unsigned int value) {
    if (arr == NULL) return false;

    int length = 0;
    while (arr[length] != 0) {
        length++;
    }

    for (int i = 0; i < length; i++) {
        if (arr[i] == value) {
            return true;
        }
    }

    return false;
}
