#include "../inc/server.h"

void mx_update_language(char **data) {
    sqlite3 *db = open_db();
    char sql[256];
    bzero(sql, 256);
    char *errmsg;
    sprintf(sql, "UPDATE USERS SET LANGUAGE=%d WHERE ID=%d;", mx_atoi(data[1]), mx_atoi(data[2]));   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Update language", st);
    sqlite3_close(db);
}
