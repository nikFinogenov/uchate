#include "../inc/libmx.h"

unsigned int mx_b64d_size(unsigned int in_size) {
	return ((3*in_size)/4);
}
