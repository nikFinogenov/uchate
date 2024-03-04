#include "../inc/server.h"

void mx_update_user_data(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    bzero(sql, 500);
    char *errmsg;
    sprintf(sql, "UPDATE USERS SET NAME='%s',"
            "SURENAME='%s',"
            "PSEUDONIM='%s',"
            "DESCRIPTION='%s' WHERE ID=%d;",
            data[1], data[2], data[3], data[4], mx_atoi(data[5]));   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Update user data", st);
    sqlite3_close(db);
}
