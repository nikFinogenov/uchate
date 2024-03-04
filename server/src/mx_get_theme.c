#include "../inc/server.h"

void mx_get_theme(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
    sprintf(sql, "SELECT THEME FROM USERS WHERE ID=%d;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);
    int theme = (int)sqlite3_column_int(res, 0);
    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, &theme, sizeof(int), 0);
}
