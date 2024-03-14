#include "libmx.h"

char *mx_double_to_str(double number) {
    char *res = NULL;
    int a1 = (int)number;
    int a2 = (int)((number - a1) * 10);
    if (a2 == 0)
        res = mx_itoa(a1);
    else
        res = mx_strjoin(mx_strjoin(mx_itoa(a1), "."), mx_itoa(a2));

    return res;
}
