#include "../inc/libmx.h" 

unsigned long mx_hex_to_nbr(const char *hex) {
    unsigned long result = 0;
    int i = 0;

    while (hex[i]) {
        char c = hex[i];
        if (c >= '0' 
            && c <= '9') {
            result = result * 16 + (c - '0');
        } else if (c >= 'A' 
            && c <= 'F') {
            result = result * 16 + (c - 'A' + 10);
        } else if (c >= 'a' 
            && c <= 'f') {
            result = result * 16 + (c - 'a' + 10);
        } else {
            break; 
        }

        i++;
    }

    return result;
}
