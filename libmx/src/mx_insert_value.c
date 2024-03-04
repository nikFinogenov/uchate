#include "libmx.h"

void mx_insert_value(unsigned int **arr, unsigned int insert_value) {
    if (*arr == NULL) {
        *arr = (unsigned int *)malloc(1);
        (*arr)[0] = insert_value;
    }
    int len = 0;
    while (arr[len] != 0) {
        len++;
    }
    unsigned int *result = (unsigned int *)malloc(len + 1);
    int i = 0;
    
    for (; i < len; i++) result[i] = (*arr)[i];
    result[i] = insert_value;
    free(*arr);
    *arr = result;
}
