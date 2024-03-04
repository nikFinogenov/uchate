#include "../inc/server.h"

void mx_check_last_room(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int last_uid = 0;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    bzero(sql, 250);
    sprintf(sql, "SELECT addresser, destination FROM Messages\
            WHERE addresser=%d OR destination=%d ORDER BY rowid DESC LIMIT 1;",
            uid, uid);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) {
        int addresser = (int)sqlite3_column_int64(res, 0);
        int destination = (int)sqlite3_column_int64(res, 1);
        if (addresser != uid)
            last_uid = addresser;
        else
            last_uid = destination;
    }
    else {
        sqlite3_finalize(res);
        sqlite3_close(db);
        send(sockfd, &last_uid, sizeof(int), 0);
        return;
    }
    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, &last_uid, sizeof(int), 0);
}
