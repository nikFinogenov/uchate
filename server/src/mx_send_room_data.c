#include "../inc/server.h"

void mx_send_room_data(char **data, int sockfd) {
    char *sendBuff = NULL;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    bzero(sql, 250);
    sprintf(sql, "SELECT NAME, SURENAME, PSEUDONIM FROM USERS\
            WHERE id=%u;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);

    sendBuff = mx_strjoin(sendBuff, (char *)sqlite3_column_text(res, 0));
    sendBuff = mx_strjoin(sendBuff, (char *)sqlite3_column_text(res, 1));
    sendBuff = mx_strjoin(sendBuff, "\n");

    char *tmp_preview = mx_strdup((char *)sqlite3_column_text(res, 2));
    tmp_preview = mx_strjoin("@", tmp_preview);
    sendBuff = mx_strjoin(sendBuff, tmp_preview);
    free(tmp_preview);

    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, sendBuff, mx_strlen(sendBuff), 0);
    free(sendBuff);
}
