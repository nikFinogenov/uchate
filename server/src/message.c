#include "server.h"

/*
    data - recv data from client
    data[0] - Operation
    data[1] - addresser
    data[2] - destination
    data[3] - time
    data[4...n-1] - text
    data[n] - NULL
*/

void mx_edit_message(char **data) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int id = mx_atoi(data[3]);

    sqlite3 *db = open_db();
    char *err_msg = 0;
    char sql[300];
    memset(sql, 0, 300);
    sprintf(sql, "UPDATE Messages SET Text='%s' WHERE id=%d AND\
            ((addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d));", 
            data[4], id, dst, uid, dst, uid);
    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Edit message", st, err_msg);
    sqlite3_close(db);
}

void mx_delete_message(char **data) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int id = mx_atoi(data[3]);

    sqlite3 *db = open_db();
    char *err_msg = 0;
    char sql[300];
    memset(sql, 0, 300);
    sprintf(sql, "DELETE FROM Messages WHERE id=%u AND\
            ((addresser=%u OR addresser=%u) AND (destination=%u OR destination=%u));", 
            id, dst, uid, dst, uid);
    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Delete sender message", st, "");

    sqlite3_stmt *res;
    memset(sql, 0, 300);
    sprintf(sql, "SELECT id FROM Messages WHERE id > %u AND\
            ((addresser=%u OR addresser=%u) AND (destination=%u OR destination=%u));", 
            id, dst, uid, dst, uid);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        unsigned int value = (unsigned int)sqlite3_column_int64(res, 0);
        sprintf(sql, "UPDATE Messages SET id=%u WHERE id=%u AND\
            ((addresser=%u OR addresser=%u) AND (destination=%u OR destination=%u));", 
            value - 1, value, dst, uid, dst, uid);
        exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
        st = (exit == 0) ? ST_OK : ST_NEOK;
        logger("Delete recipient message", st, err_msg);
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
}

void mx_insert_message(char **data, int sockfd) {
    char *text = NULL;
    for (int i = 4; data[i] != NULL; i++)
        text = mx_strjoin(text, data[i]);

    int addresser = mx_atoi(data[1]);
    int destination = mx_atoi(data[2]);

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[2056];
    memset(sql, 0, 2056);
    sprintf(sql, "SELECT MAX(ID) FROM Messages WHERE (addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d);",
        addresser, destination, addresser, destination);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);
    int id = (int)sqlite3_column_int(res, 0);
    sqlite3_finalize(res);
    id++;

    send(sockfd, &id, sizeof(int), 0);

    char *err_msg;
    memset(sql, 0, 2056);
    if (mx_strcmp(text, "(null)"))
        sprintf(sql,
                "INSERT INTO Messages (id, addresser, destination, Text, time)\
                VALUES('%d','%d','%d','%s','%d');",
                id, addresser, destination, text, mx_atoi(data[3]));
    else
        sprintf(sql,
                "INSERT INTO Messages (id, addresser, destination, time)\
                VALUES('%d','%d','%d','%d');",
                id, addresser, destination, mx_atoi(data[3]));

    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Insert message", st, err_msg);
    sqlite3_close(db);

    free(text);
}

void mx_check_messages(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);

    int latest = 0;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    memset(sql, 0, 250);
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
