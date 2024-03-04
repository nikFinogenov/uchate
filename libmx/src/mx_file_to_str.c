#include "../inc/libmx.h"

char *mx_file_to_str(const char *file){

    int file_opened = open(file, O_RDONLY);

    if(file_opened == -1){
        close(file_opened);
        return NULL;
    }

    int read_fl = 0;
    int len = 0;
    char c;
    read_fl = read(file_opened, &c, 1);
    while (read_fl > 0) {
        read_fl = read(file_opened, &c, 1);
        len++;
    }
    close(file_opened);

    file_opened = open(file, O_RDONLY);
    if(!len)
        return NULL;
    
    char *res = mx_strnew(len);
    read(file_opened, res, len);
    close(file_opened);
    return res;
}
