#include "../inc/server.h"

int mx_uint_array_insert(unsigned int **arr, unsigned int insert_value, int len) {
    if (*arr == NULL) {
        *arr = (unsigned int *)malloc(1);
        (*arr)[0] = insert_value;
        return 1;
    }    
    unsigned int *result = (unsigned int *)malloc(len + 1);
    int i = 0;
    for (; i < len; i++) result[i] = (*arr)[i];
    result[i] = insert_value;
    free(*arr);
    *arr = result;
    return len + 1;
}
