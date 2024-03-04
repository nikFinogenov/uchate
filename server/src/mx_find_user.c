#include "../inc/server.h"

void mx_find_user(char **data, int sockfd) {
    char *sendBuff = NULL;

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
    sprintf(sql, "SELECT ID FROM USERS WHERE PSEUDONIM='%s';", data[1]);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) sendBuff = mx_strjoin(sendBuff, "SUCCESS");
    else sendBuff = mx_strjoin(sendBuff, "FAIL");
    sqlite3_finalize(res);
    sqlite3_close(db);
    send(sockfd, sendBuff, strlen(sendBuff), 0);
    free(sendBuff);
}
