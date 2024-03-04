#include "../inc/server.h"

bool mx_uint_arr_check_value(unsigned int *arr, unsigned int value, int len) {
    if (arr == NULL)
        return false;
    
    for (int i = 0; i < len; i++)
        if (arr[i] == value)
            return true;
            
    return false;
}
