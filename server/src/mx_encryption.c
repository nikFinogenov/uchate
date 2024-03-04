#include "../inc/server.h"

char *mx_encryption(char *str) {
    int len = mx_strlen(str);
    for (int i = 0; str[i]; i++) {
        if (str[i] == str[0])
            str[i] = str[i] + 1;
        else if (str[i] == str[len - 1])
            str[i] = str[i] + 3;
        else if (i % 2 == 0) 
            str[i] = str[i] + 2;
        else {
            str[i] = str[i] - 2;
        }
    }
    return str;
}
