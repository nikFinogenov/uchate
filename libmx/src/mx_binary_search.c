#include "../inc/libmx.h"

int mx_binary_search(char **arr, int size, const char *s, int *count) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int middle = left + (right - left) / 2;
        (*count)++;
        int cmp = mx_strcmp(arr[middle], s);

        if (cmp == 0)
            return middle;
        else if (cmp > 0)
            right = middle - 1;
        else
            left = middle + 1;
    }

    *count = 0;
    return -1;
}
