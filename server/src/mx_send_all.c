#include "../inc/server.h"

void mx_send_all(int *socket, void *buffer, size_t length) {
    unsigned char *ptr = (unsigned char *)buffer;
    while (length > 0) {
        usleep(70000);
        int i = send(*socket, ptr, length, 0);
        ptr += i;
        length -= i;
    }
}
