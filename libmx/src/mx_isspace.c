#include "../inc/libmx.h"

int mx_isspace(char c) {
	if (c == '\n' || c == '\t' || c == '\v' ||
		 c == '\r' || c == '\f' || c == ' ')
		return 1;
	return 0;
}
