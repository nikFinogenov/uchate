#include "server.h"

void mx_create_chat(char **data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char response[DEFAULT_MESSAGE_SIZE];
    char *errmsg;
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

    int offset = 0; 

    while (sqlite3_step(res) == SQLITE_ROW) {
        const unsigned char *interlocutor_username = sqlite3_column_text(res, 0);

        offset += sprintf(temp_buff + offset, "%s\n", interlocutor_username);
    }

    int exit = sqlite3_finalize(res);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    // logger("Get chat", st, "");
    if (strlen(temp_buff) == 0) {
        send(sockfd, "1", strlen("1"), 0);
    }
    else send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
}

void mx_update_reload_status(char** data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, sizeof(sql));
    char response[DEFAULT_MESSAGE_SIZE];
    char *errmsg;
    memset(sql, 0, sizeof(sql));
    int chat_id = get_chat_id(data[2], data[3]);
    sprintf(sql, "UPDATE CHATS SET reload='%s' WHERE id=%d;",data[1], chat_id);   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == SQLITE_OK) ? ST_OK : ST_NEOK;
    logger("Update reload", st, errmsg);
    sqlite3_close(db);
    if (exit == SQLITE_OK) 
        sprintf(response, "0");
    else 
        sprintf(response, "1");
    send(sockfd, response, strlen(response), 0);
}

void mx_get_reload_status(char** data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];

    memset(sql, 0, 500);
    char temp_buff[DEFAULT_MESSAGE_SIZE];
    memset(temp_buff, 0, DEFAULT_MESSAGE_SIZE);
    int chat_id = get_chat_id(data[1], data[2]);
    sprintf(sql, "SELECT reload FROM CHATS WHERE id = %d;",chat_id); 
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) == SQLITE_ROW) {
        const unsigned char *status = sqlite3_column_text(res, 0);
        sprintf(temp_buff, "%s\n", status);
    }
    int exit = sqlite3_finalize(res);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Get chat status", st, "");
    if(mx_strlen(temp_buff) == 0) send(sockfd, "false", strlen("false"), 0);
    else send(sockfd, temp_buff, strlen(temp_buff), 0);
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

void mx_delete_chat(char **data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    char *errmsg;

    int chat_id = get_chat_id(data[1], data[2]);

    sprintf(sql, "DELETE FROM MESSAGES WHERE chat_id=%d;", chat_id);
    int exit_messages = sqlite3_exec(db, sql, NULL, 0, &errmsg);

    sprintf(sql, "DELETE FROM CHATS WHERE id=%d;", chat_id);
    int exit_chats = sqlite3_exec(db, sql, NULL, 0, &errmsg);

    char *st_messages = (exit_messages == 0) ? ST_OK : ST_NEOK;
    char *st_chats = (exit_chats == 0) ? ST_OK : ST_NEOK;

    // logger("Delete chat messages", st_messages, errmsg);
    logger("Delete chat", st_chats, errmsg);

    char response[2] = {(exit_messages == 0 && exit_chats == 0) ? '0' : '1', '\0'};
    send(sockfd, response, strlen(response), 0);

    sqlite3_close(db);
}
