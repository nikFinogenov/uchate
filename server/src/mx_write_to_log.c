#include "server.h"

void logger(char *proccess, char* status) {
    FILE *fd = fopen("server.log", "a+t");
    if (fd == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t current_time = time(NULL);
    char current_time_str[20];
    strftime(current_time_str, sizeof(current_time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // char* input_color = (!mx_strcmp(status, ST_OK)) ? GREEN : RED;
    fprintf(fd, "%s: %s -> %s\n", current_time_str, proccess, status);

    if (fclose(fd) != 0) {
        perror("Error closing log file");
    }
}
