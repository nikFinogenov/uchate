#include "../inc/server.h"

void mx_recv_all(int *socket, unsigned char **buffer, size_t length) {
    unsigned char *ptr = *buffer;
    while (length > 0) {
        int i = recv(*socket, ptr, length, 0);
        ptr += i;
        length -= i;
    }
}
