#include "../inc/libmx.h"

unsigned int b64e_size(unsigned int in_size) {
	unsigned int i, j = 0;
	for (i=0;i<in_size;i++) {
		if (i % 3 == 0)
		j += 1;
	}
	return (4*j);
}
