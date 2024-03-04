#include "../inc/server.h"

void mx_update_theme(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    bzero(sql, 500);
    char *err_msg;
    sprintf(sql, "UPDATE USERS SET THEME=%d WHERE ID=%d;", mx_atoi(data[1]), mx_atoi(data[2]));
    sqlite3_exec(db, sql, NULL, 0, &err_msg);
    sqlite3_close(db);
}
