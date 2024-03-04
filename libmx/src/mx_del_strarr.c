#include "../inc/libmx.h"

void mx_del_strarr(char ***arr) {
	if (arr != NULL && *arr != NULL) {
        char **current_string = *arr;
        while (*current_string != NULL) {
            mx_strdel(current_string);
            current_string++;
        }

        free(*arr);  
        *arr = NULL;   
    }
}
