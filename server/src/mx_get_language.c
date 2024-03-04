#include "../inc/server.h"

void mx_get_language(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[256];
    bzero(sql, 256);
    sprintf(sql, "SELECT LANGUAGE FROM USERS WHERE ID=%d;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);
    int language = (int)sqlite3_column_int(res, 0);
    send(sockfd, &language, sizeof(int), 0);
    sqlite3_finalize(res);
    sqlite3_close(db);
}
