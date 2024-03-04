#include "../inc/server.h"

void mx_load_room(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int id = 0;

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[250];
    bzero(sql, 250);
    sprintf(sql, "SELECT MAX(ID) FROM Messages\
            WHERE (addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d);",
            uid, dst, uid, dst);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) {
        id = (int)sqlite3_column_int(res, 0);
        send(sockfd, &id, sizeof(int), 0);
    }
    else {
        sqlite3_finalize(res);
        sqlite3_close(db);
        send(sockfd, &id, sizeof(int), 0);
        return;
    }
    sqlite3_finalize(res);
    bzero(sql, 250);
    sprintf(sql, "SELECT id, addresser, Text, time FROM Messages\
            WHERE (addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d)\
            ORDER BY id;",
            uid, dst, uid, dst);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        char *message_text = (char *)sqlite3_column_text(res, 2);

        int cur_m_id = (int)sqlite3_column_int64(res, 0);

        char sendBuff[1024];
        bzero(sendBuff, 1024);
        sprintf(sendBuff, "%d\n%d\n%s\n%d", cur_m_id, 
            (int)sqlite3_column_int64(res, 1), message_text, (int)sqlite3_column_int64(res, 3));
        send(sockfd, sendBuff, 1024, 0);

        send(sockfd, &cur_m_id, sizeof(int), 0);
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
}
