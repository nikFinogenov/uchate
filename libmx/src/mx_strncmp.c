#include "../inc/libmx.h"

int mx_strncmp(const char *s1, const char *s2, int n) {
    int i = 0;
	
    while ( s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i] && i < n-1)
        i++;
    return s1[i]-s2[i];
}
