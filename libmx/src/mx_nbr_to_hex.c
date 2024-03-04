#include "../inc/libmx.h" 

char* mx_nbr_to_hex(unsigned long nbr) {

    int max_digits = 0;
    unsigned long temp = nbr;
    while (temp > 0) {
        max_digits++;
        temp /= 16;
    }

    char* hex_string = (char*)malloc((max_digits + 1) * 1);
    if (hex_string == NULL) {
        return NULL;
    }

    int i = max_digits - 1;
    while (nbr > 0) {
        unsigned long remainder = nbr % 16;
        if (remainder < 10) {
            hex_string[i] = '0' + remainder;
        } else {
            hex_string[i] = 'a' + (remainder - 10);
        }
        nbr /= 16;
        i--;
    }

    hex_string[max_digits] = '\0';
    return hex_string;
}
