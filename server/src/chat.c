#include "server.h"

void mx_create_chat(char **data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char response[DEFAULT_MESSAGE_SIZE];
    char *errmsg;
        // sprintf(sql, "INSERT INTO CHATS (user1_id, user2_id) \
        //     VALUES (SELECT id from USERS where username = '%s',SELECT id from USERS where username = '%s');", data[1], data[2]);
    sprintf(sql, "INSERT INTO CHATS (user1_id, user2_id) \
    VALUES ((SELECT id from USERS where username = '%s'), (SELECT id from USERS where username = '%s'));", data[1], data[2]);

    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Create chat", st, errmsg);
    sqlite3_close(db);
    if(exit == 0) sprintf(response, "0");
    else sprintf(response, "1");
    send(sockfd, response, strlen(response), 0);
}

void mx_get_chat(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[500];
    memset(sql, 0, 500);
    char temp_buff[1024];
    memset(temp_buff, 0, 1024);
    sprintf(sql, "SELECT u.username AS interlocutor_username "
           "FROM CHATS c "
           "JOIN USERS u ON (c.user1_id = u.id OR c.user2_id = u.id) "
           "WHERE (c.user1_id = (SELECT id FROM USERS WHERE username = '%s') "
           "       OR c.user2_id = (SELECT id FROM USERS WHERE username = '%s')) "
           "       AND u.username != '%s';",
           data[1], data[1], data[1]);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);

    int offset = 0; // Смещение для записи в буфер

    while (sqlite3_step(res) == SQLITE_ROW) {
    // Get the username of the interlocutor
        const unsigned char *interlocutor_username = sqlite3_column_text(res, 0);

    // Print the username to the buffer
        offset += sprintf(temp_buff + offset, "%s\n", interlocutor_username);
    }

    int exit = sqlite3_finalize(res);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Get chat", st, "");
    if (strlen(temp_buff) == 0) {
        send(sockfd, "1", strlen("1"), 0);
    }
    else send(sockfd, temp_buff, strlen(temp_buff), 0);
    // Отправляем весь буфер клиенту
    sqlite3_close(db);
}



void mx_update_chat(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    sprintf(sql, "UPDATE CHATS SET user1_id=%s, user2_id=%s, \
            WHERE id=%d;",
            data[1], data[2], mx_atoi(data[3]));

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
