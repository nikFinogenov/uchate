#include "../inc/libmx.h"

int mx_atoi(const char *str) {
    int i;
    long long check = 0;
    if(str[0] == '-') {
         if(str[1] == 45 
         || !(str[1] >= 48 && str[1] <= 57) 
         || (str[1] == ' ' 
         || str[1] == '\t' 
         || str[1] == '\n' 
         || str[1] == '\v' 
         || str[1] == '\f' 
         || str[1] == '\r') ) {
            return 0;
        }
        for (i = 1; (str[i] >= 48 && str[i] <= 57) 
        && !(str[i] == ' ' 
        || str[i] == '\t' 
        || str[i] == '\n' 
        || str[i] == '\v' 
        || str[i] == '\f' 
        || str[i] == '\r'); i++) {
            check = (str[i] - '0') + (10 * check);
        }
        while(check < -2147483647 - 1) {
            check = check + 2147483647 + 2147483647;
        }
        return -(int)check;
    }
    for (i = 0; ((str[i] >= 48 && str[i] <= 57) 
    && !(str[i] == ' ' 
    || str[i] == '\t' 
    || str[i] == '\n' 
    || str[i] == '\v' 
    || str[i] == '\f' 
    || str[i] == '\r')); i++) {
        check = (check * 10) + (str[i] - '0');
    }
    while(check >= 2147483647) {
        return -1;
    }
    return (int)check;
}
