#include "../inc/libmx.h"

void mx_swap_str(char **s1, char **s2) {
	char *buf = *s1;
	*s1 = *s2;
	*s2 = buf;
}
