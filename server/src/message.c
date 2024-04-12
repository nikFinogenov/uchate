#include "server.h"

// /*
//     data - recv data from client
//     data[0] - Operation
//     data[1] - addresser
//     data[2] - destination
//     data[3] - time
//     data[4...n-1] - text
//     data[n] - NULL
// */

static int get_chat_id(char *username_1, char *username_2) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[500];
    memset(sql, 0, 500);
    int chat_id = -1; // Initialize chat_id to a default value

    sprintf(sql, "SELECT c.id "
                 "FROM CHATS c "
                 "JOIN USERS u1 ON c.user1_id = u1.id "
                 "JOIN USERS u2 ON c.user2_id = u2.id "
                 "WHERE (u1.username = '%s' AND u2.username = '%s') "
                 "OR (u1.username = '%s' AND u2.username = '%s');",
            username_1, username_2, username_2, username_1);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (sqlite3_step(res) == SQLITE_ROW) {
        // Get the chat_id
        chat_id = sqlite3_column_int(res, 0);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
    
    return chat_id;
}

void mx_add_message(char **data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    int chat_id = get_chat_id(data[1], data[2]);

    sprintf(sql, "INSERT INTO MESSAGES (chat_id, text, type, date) \
            VALUES (%d, '%s', 'text', '%s');",
            chat_id, data[3], data[4]);

    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    if (exit != SQLITE_OK) {
        logger("Add message", ST_NEOK, errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        // Handle error
        return;
    }

    // Get the ID of the last inserted row
    sqlite3_int64 message_id = sqlite3_last_insert_rowid(db);
    sqlite3_close(db);
    // Convert the message ID to a string
    char message_id_str[50];
    snprintf(message_id_str, sizeof(message_id_str), "%" PRId64, (int64_t)message_id);

    // Send the message ID back to the client
    send(sockfd, message_id_str, strlen(message_id_str), 0);
}

void mx_get_message(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    memset(sql, 0, 500);
    char temp_buff[1024];
    memset(temp_buff, 0, 1024);

    sprintf(sql, "SELECT * FROM MESSAGES WHERE ID = %d;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);

    while (sqlite3_step(res) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        int chat_id = sqlite3_column_int(res, 1);
        const unsigned char *type = sqlite3_column_text(res, 2);
        const unsigned char *status = sqlite3_column_text(res, 3);
        const unsigned char *text = sqlite3_column_text(res, 4);

        sprintf(temp_buff, "%d\n%d\n%s\n%s\n%s\n", id, chat_id, type, status, text);
    }

    int exit = sqlite3_finalize(res);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Get message", st, "");
    send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
}

void mx_update_message(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    sprintf(sql, "UPDATE MESSAGES SET chat_id=%s, type='%s', status='%s', \
            text='%s' WHERE id=%d;",
            data[1], data[2], data[3], data[4], mx_atoi(data[5]));

    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Update message", st, errmsg);
    sqlite3_close(db);
}

void mx_delete_message(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    sprintf(sql, "DELETE FROM MESSAGES WHERE id=%d;", mx_atoi(data[1]));

    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char *st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Delete message", st, errmsg);
    sqlite3_close(db);
}


void mx_check_MESSAGES(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);

    int latest = 0;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    memset(sql, 0, 250);
    sprintf(sql, "SELECT MAX(id) FROM MESSAGES\
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
