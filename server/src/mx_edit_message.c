#include "../inc/server.h"

void mx_edit_message(char **data) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int id = mx_atoi(data[3]);

    sqlite3 *db = open_db();
    char *err_msg = 0;
    char sql[300];
    bzero(sql, 300);
    sprintf(sql, "UPDATE Messages SET Text='%s' WHERE id=%d AND\
            ((addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d));", 
            data[4], id, dst, uid, dst, uid);
    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Edit message", st);
    sqlite3_close(db);
}
