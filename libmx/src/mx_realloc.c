#include "../inc/libmx.h"

void *mx_realloc(void *ptr, size_t size) {
    
    if (!size && ptr) {
        free(ptr);
        return NULL;
    }

    if (!ptr && size)
        return malloc(size);

    size_t ptr_size = malloc_size(ptr); 

    if (ptr_size > size)
        size = ptr_size;
    
    void *new = (void *) malloc(size);

    if (!new)
        return NULL;
    
    new = mx_memmove(new, ptr, ptr_size);

    free(ptr);
    return new;
}
