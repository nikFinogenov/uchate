#include "../inc/libmx.h"

int mx_sscanf(const char *str, const char *format, char *flag, char *directory) {
    int parsed_values = 0;

    if (flag) {
        *flag = '\0';
    }
    if (directory) {
        *directory = '\0';
    }

    while (*format != '\0') {
        if (*format == '%') {
            format++;
            if (*format == 's') {
                int i = 0;
                char *city = (parsed_values == 0 && flag) ? flag : directory;
                while (*str != '\0' && *str != ' ' && *str != ',' && *str != '-') {
                    if (city) {
                        city[i] = *str;
                    }
                    i++;
                    str++;
                }
                if (city) {
                    city[i] = '\0';
                }
                parsed_values++;
            }
        } else {
            if (*str != *format) {
                return parsed_values;
            }
            str++;
        }
        format++;
    }

    return parsed_values;
}
