#include "server.h"

void mx_add_user(char **data, int sockfd) {
    char *encrypted_pass = encrypt_pass(mx_strdup(data[2]));

    char response[DEFAULT_MESSAGE_SIZE];
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    memset(sql, 0, 500);
    char *errmsg;

    sprintf(sql, "SELECT id FROM USERS WHERE username = '%s';", data[1]);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    int result = sqlite3_step(res);
    sqlite3_finalize(res);

    if (result == SQLITE_ROW) {
        sprintf(response, "1");
    } else {
        sprintf(sql, "SELECT id FROM USERS ORDER BY id DESC LIMIT 1;");
        sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_step(res);
        int id = sqlite3_column_int(res, 0) + 1;
        sqlite3_finalize(res);
        
        sprintf(sql, 
                "INSERT INTO USERS (username, password, name, surname, description, status, date, token) \
                VALUES( '%s','%s','%s','%s','%s','%s','%s','%s');", 
                data[1], encrypted_pass, data[3], data[4], " ", data[5], " ", " ");   
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

void mx_update_user_status(char** data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, sizeof(sql));
    char response[DEFAULT_MESSAGE_SIZE];
    char *errmsg;
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "UPDATE USERS SET status='%s' WHERE username='%s';",
            data[1], data[2]);   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == SQLITE_OK) ? ST_OK : ST_NEOK;
    logger("Update user", st, errmsg);
    sqlite3_close(db);
    if (exit == SQLITE_OK) 
        sprintf(response, "0");
    else 
        sprintf(response, "1");
    send(sockfd, response, strlen(response), 0);
}

void mx_get_user_status(char** data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];

    memset(sql, 0, 500);
    char temp_buff[DEFAULT_MESSAGE_SIZE];
    memset(temp_buff, 0, DEFAULT_MESSAGE_SIZE);

    sprintf(sql, "SELECT status FROM USERS WHERE username = '%s';", data[1]); 
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) == SQLITE_ROW) {
        const unsigned char *status = sqlite3_column_text(res, 0);
        sprintf(temp_buff, "%s\n", status);
    }
    int exit = sqlite3_finalize(res);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Get user status", st, "");
    send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
}

void mx_update_avatar(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    int rc;
    
    FILE *file = fopen(data[1], "rb");
    if (!file) {
        printf("Error opening file %s\n", data[1]);
        sqlite3_close(db);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *avatar_data = (unsigned char*)malloc(file_size);
    if (!avatar_data) {
        printf("Error allocating memory for avatar data\n");
        fclose(file);
        sqlite3_close(db);
        return;
    }

    fread(avatar_data, 1, file_size, file);
    fclose(file);

    const char *sql = "UPDATE USERS SET profile_img=? WHERE username=?";
    
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        free(avatar_data);
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_blob(res, 1, avatar_data, file_size, SQLITE_STATIC);
    sqlite3_bind_text(res, 2, data[2], -1, SQLITE_STATIC);

    rc = sqlite3_step(res);
    if (rc != SQLITE_DONE) {
        printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
        free(avatar_data);
        sqlite3_finalize(res);
        sqlite3_close(db);
        return;
    }

    sqlite3_finalize(res);
    free(avatar_data);
    sqlite3_close(db);

    const char *response = "Avatar updated successfully";
    send(sockfd, response, strlen(response), 0);
}

void mx_get_user_avatar(char** data, int sockfd) {
    if (data == NULL || data[1] == NULL) {
        logger("Get user avatar", ST_NEOK, "Invalid data");
        return;
    }

    sqlite3* db = open_db();
    if (db == NULL) {
        logger("Get user avatar", ST_NEOK, "Failed to open database");
        return;
    }

    sqlite3_stmt* res;
    char sql[500];
    char temp_buff[DEFAULT_MESSAGE_SIZE];
    memset(sql, 0, 500);
    memset(temp_buff, 0, DEFAULT_MESSAGE_SIZE);

    sprintf(sql, "SELECT profile_img FROM USERS WHERE username = '%s';", data[1]); 
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        logger("Get user avatar", ST_NEOK, "Failed to prepare SQL statement");
        sqlite3_close(db);
        return;
    }

    while (sqlite3_step(res) == SQLITE_ROW) {
        const void* blob_data = sqlite3_column_blob(res, 0);
        int blob_size = sqlite3_column_bytes(res, 0);

        if (send(sockfd, &blob_size, sizeof(blob_size), 0) == -1) {
            perror("Error sending data size through socket");
            logger("Send avatar size", ST_NEOK, "Failed to send avatar size through socket");
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }

        if (send(sockfd, blob_data, blob_size, 0) == -1) {
            perror("Error sending data through socket");
            logger("Send avatar data", ST_NEOK, "Failed to send avatar data through socket");
            sqlite3_finalize(res);
            sqlite3_close(db);
            return;
        }
    }

    int exit = sqlite3_finalize(res);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Get user avatar", st, "");
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
            strcpy(temp_buff, "1");
        }
        sqlite3_finalize(res);
    } else {
        strcpy(temp_buff, "-1");
    }

    logger("Get user", temp_buff, "");
    send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
}

static int username_exists_callback(void *not_used, int argc, char **argv, char **az_col_name) {
    return 1;
}

void mx_update_user(char **data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, sizeof(sql));
    char response[DEFAULT_MESSAGE_SIZE];
    char *errmsg;
    bool username_exists = false;

    sprintf(sql, "SELECT * FROM USERS WHERE username='%s';", data[1]);
    int result = sqlite3_exec(db, sql, username_exists_callback, 0, NULL);

    if (result != SQLITE_OK) {
        logger("Error checking username existence", ST_NEOK, sqlite3_errmsg(db));
        sqlite3_close(db);
        sprintf(response, "Error checking username existence");
        send(sockfd, response, strlen(response), 0);
        return;
    }

    if (username_exists) {
        logger("Username already exists", ST_NEOK, "Username already exists in the database");
        sprintf(response, "Username already exists");
        send(sockfd, response, strlen(response), 0);
        sqlite3_close(db);
        return;
    }

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "UPDATE USERS SET username='%s', name='%s', \
            surname='%s', description='%s' WHERE username='%s';",
            data[1], data[2], data[3], data[4], data[5]);   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == SQLITE_OK) ? ST_OK : ST_NEOK;
    logger("Update user", st, errmsg);
    sqlite3_close(db);
    if (exit == SQLITE_OK) 
        sprintf(response, "0");
    else 
        sprintf(response, "1");
    send(sockfd, response, strlen(response), 0);
}



void mx_delete_user(char **data) {
    sqlite3 *db = open_db();
    char sql[500];

    memset(sql, 0, 500);
    char *errmsg;
    sprintf(sql, "DELETE FROM USERS WHERE id=%d;", mx_atoi(data[1]));   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Delete user", st, errmsg);
    sqlite3_close(db);
}

bool mx_check_user(char **data) {
    char *encrypted_pass = encrypt_pass(mx_strdup(data[2]));

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];

    memset(sql, 0, 500);
    sprintf(sql, "SELECT password FROM USERS WHERE USERNAME='%s';", data[1]);
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
