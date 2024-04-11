#include "server.h"

void mx_add_user(char **data, int sockfd) {
    char *encrypted_pass = encrypt_pass(mx_strdup(data[2]));

    char response[DEFAULT_MESSAGE_SIZE];
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    // Check if user with given username already exists
    sprintf(sql, "SELECT id FROM USERS WHERE username = '%s';", data[1]);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    int result = sqlite3_step(res);
    sqlite3_finalize(res);

    if (result == SQLITE_ROW) {
        // User with the given username already exists
        sprintf(response, "1");
    } else {
        // User does not exist, add new user
        sprintf(sql, "SELECT id FROM USERS ORDER BY id DESC LIMIT 1;");
        sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_step(res);
        int id = sqlite3_column_int(res, 0) + 1;
        sqlite3_finalize(res);
        
        sprintf(sql, 
                "INSERT INTO USERS (username, password, name, surname, description, status, date, token) \
                VALUES( '%s','%s','%s','%s','%s','%s','%s','%s');", 
                data[1], encrypted_pass, data[3], data[4], " ", " ", " ", " ");   
        int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
        char* st = (exit == 0) ? ST_OK : ST_NEOK;
        logger("Add user", st, errmsg);

        mx_write_photo_to_bd("server/source/standard_avatar.png", id);
        sprintf(response, "0");
    }

    sqlite3_close(db);
    free(encrypted_pass);

    send(sockfd, response, strlen(response), 0);
}

void mx_get_user(char** data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];

    memset(sql, 0, 500);
    char temp_buff[DEFAULT_MESSAGE_SIZE];
    memset(temp_buff, 0, DEFAULT_MESSAGE_SIZE);

    sprintf(sql, "SELECT username, name, surname, description FROM USERS WHERE username = '%s';", data[1]); 
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) == SQLITE_ROW) {
        const unsigned char *username = sqlite3_column_text(res, 0);
        // const unsigned char *password = sqlite3_column_text(res, 1);
        const unsigned char *name = sqlite3_column_text(res, 1);
        const unsigned char *surname = sqlite3_column_text(res, 2);
        const unsigned char *desc = sqlite3_column_text(res, 3);
        sprintf(temp_buff, "%s\n%s\n%s\n%s\n", username, name, surname, desc);
    }
    int exit = sqlite3_finalize(res);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Get user", st, "");
    send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
}

void mx_get_chatter(char** data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    char temp_buff[DEFAULT_MESSAGE_SIZE];

    memset(sql, 0, sizeof(sql));
    memset(temp_buff, 0, sizeof(temp_buff));

    sprintf(sql, "SELECT username, name, surname FROM USERS WHERE username = '%s';", data[1]);

    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {
        int step = sqlite3_step(res);
        if (step == SQLITE_ROW) {
            const unsigned char *username = sqlite3_column_text(res, 0);
            const unsigned char *name = sqlite3_column_text(res, 1);
            const unsigned char *surname = sqlite3_column_text(res, 2);
            sprintf(temp_buff, "%s\n%s\n%s\n", username, name, surname);
        } else {
            strcpy(temp_buff, "1"); // User not found
        }
        sqlite3_finalize(res);
    } else {
        strcpy(temp_buff, "-1"); // Error in SQL execution
    }

    logger("Get user", temp_buff, "");
    send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
}

void mx_update_user(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;
    char* enc_pass = encrypt_pass(data[1]);
    sprintf(sql, "UPDATE USERS SET password='%s', name='%s', \
            surname='%s', description='%s' WHERE id=%d;",
            enc_pass, data[2], data[3], data[4], mx_atoi(data[5]));   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    // logger(errmsg, st);
    logger("Update user", st, errmsg);
    sqlite3_close(db);
}

void mx_delete_user(char **data) {
    sqlite3 *db = open_db();
    char sql[500];

    memset(sql, 0, 500);
    char *errmsg;
    sprintf(sql, "DELETE FROM USERS WHERE id=%d;", mx_atoi(data[1]));   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    // logger(errmsg, st);
    logger("Delete user", st, errmsg);
    sqlite3_close(db);
}

bool mx_check_user(char **data) {
    char *encrypted_pass = encrypt_pass(mx_strdup(data[2]));

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];

    memset(sql, 0, 500);
    sprintf(sql, "SELECT PASSWORD FROM USERS WHERE USERNAME='%s';", data[1]);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) {
        char *check_password = mx_strdup((char *)sqlite3_column_text(res, 0));
        if (mx_strcmp(check_password, encrypted_pass) == 0) {
            free(check_password);
            sqlite3_finalize(res);
            sqlite3_close(db);
            free(encrypted_pass);
            return true;
        }
        else {
            free(check_password);
            sqlite3_finalize(res);
            sqlite3_close(db);
            free(encrypted_pass);
            return false; 
        }
    }
    else {
        sqlite3_finalize(res);
        sqlite3_close(db);
        free(encrypted_pass);
        return false; 
    }
}

void check_login_data(char **data, int sockfd) {
    char response[DEFAULT_MESSAGE_SIZE];

    if (mx_check_user(data)) {
        mx_get_user(data, sockfd);
        return;
    } else {
        sprintf(response, "1");
    }

    send(sockfd, response, strlen(response), 0);
}
