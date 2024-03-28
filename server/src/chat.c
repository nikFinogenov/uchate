#include "server.h"

void mx_create_chat(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    sprintf(sql, "INSERT INTO CHATS (user1_id, user2_id, creation_date) \
            VALUES (%s, %s, '%s');",
            data[1], data[2], data[3]);

    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Create chat", st, errmsg);
    sqlite3_close(db);
}

void mx_get_chat(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    memset(sql, 0, 500);
    char temp_buff[1024];
    memset(temp_buff, 0, 1024);

    sprintf(sql, "SELECT * FROM CHATS WHERE ID = %d;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);

    while (sqlite3_step(res) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        int user1_id = sqlite3_column_int(res, 1);
        int user2_id = sqlite3_column_int(res, 2);
        const unsigned char *creation_date = sqlite3_column_text(res, 3);

        sprintf(temp_buff, "%d\n%d\n%d\n%s\n", id, user1_id, user2_id, creation_date);
    }

    int exit = sqlite3_finalize(res);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Get chat", st, "");
    send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
}

void mx_update_chat(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    sprintf(sql, "UPDATE CHATS SET user1_id=%s, user2_id=%s, \
            creation_date='%s' WHERE id=%d;",
            data[1], data[2], data[3], mx_atoi(data[4]));

    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Update chat", st, errmsg);
    sqlite3_close(db);
}

void mx_delete_chat(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    sprintf(sql, "DELETE FROM CHATS WHERE id=%d;", mx_atoi(data[1]));

    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Delete chat", st, errmsg);
    sqlite3_close(db);
}
