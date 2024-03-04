#include "../inc/server.h"

void mx_check_messages(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);

    int latest = 0;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    bzero(sql, 250);
    sprintf(sql, "SELECT MAX(id) FROM Messages\
            WHERE (addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d);",
            uid, dst, uid, dst);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) {
        latest = (int)sqlite3_column_int(res, 0);
        send(sockfd, &latest, sizeof(int), 0);
    }
    else {
        sqlite3_finalize(res);
        sqlite3_close(db);
        send(sockfd, &latest, sizeof(int), 0);
        return;
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
}
