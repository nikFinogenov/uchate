#include "../inc/libmx.h"

int mx_read_line(char **lineptr, size_t buf_size, char delim, const int fd) {
    if (lineptr == NULL || buf_size == 0 || fd < 0) {
        return -2;
    }

    char *buffer = (char *)mx_strnew(buf_size);
    if (buffer == NULL) {
        return -2;
    }

    size_t bytes_read = 0;

    while (1) {
        char c;
        ssize_t result = read(fd, &c, 1);

        if (result == -1) {
            mx_strdel(&buffer);
            return -2;
        } else if (result == 0 && bytes_read == 0) {
            mx_strdel(&buffer);
            return -1;
        } else if (result == 0 || c == delim) {
            break;
        } else {
            buffer[bytes_read++] = c;
        }

        if (bytes_read >= buf_size - 1) {
            buf_size *= 2;
            char *new_buffer = (char *)mx_strnew(buf_size);

            if (new_buffer == NULL) {
                mx_strdel(&buffer);
                return -2;
            }

            mx_strncpy(new_buffer, buffer, bytes_read);
            mx_strdel(&buffer);
            buffer = new_buffer;
        }
    }

    buffer[bytes_read] = '\0';
    *lineptr = buffer;
    return bytes_read;
}
