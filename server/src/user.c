#include "server.h"

/*
    data - recv data from client
    data[0] - Operation
    data[1] - NAME
    data[2] - SURNAME
    data[3] - USERNAME
    data[4] - PASSWORD
    data[5] - NULL
*/

void mx_add_user(char **data) {
    char *encrypted_pass = encrypt_pass(mx_strdup(data[4]));

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
    char *errmsg;
    // sprintf(sql, "SELECT MAX(ID) FROM USERS;");
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);
    // int id = (int)sqlite3_column_int(res, 0);
    sqlite3_finalize(res);
    // id++;
    sprintf(sql, 
            "INSERT INTO USERS (username, password, name, surname, description, status, date, token) VALUES('%d',\
            '%s','%s','%s','%s','%s','%d','%d');", 
            data[1], encrypted_pass, data[2], data[3], " ", , 0, 0);   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Add user", st);
    sqlite3_close(db);
    mx_write_photo_to_bd("server/source/standard_avatar.jpg", id);

    free(encrypted_pass);
}

void mx_get_user(int sockfd) {
    // mx_printstr(data[0]);
    // mx_printstr(data[1]);
    // mx_printstr(data[2]);
    // mx_printstr(data[3]);
    // mx_printstr(data[4]);
    // mx_printstr(data[5]);
        // printf("gettere usere");
    // printf("data1 = %s\ndata2 = %s\n data3 = %s\ndata4 = %s\ndata5 = %s\ndata6 = %s", data[0], data[1], data[2], data[3], data[4], data[5]);
    // char *encrypted_pass = encrypt_pass(mx_strdup(data[4]));
    int id = 1;

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
            char temp_buff[1024];
        bzero(temp_buff, 1024);
    // char *errmsg;
    sprintf(sql, "SELECT * FROM USERS WHERE ID = %d;", id); 
    // const char *query = "SELECT * FROM Users WHERE id = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
    }
    // sqlite3_bind_int(res, 1, 1);

    while (sqlite3_step(res) == SQLITE_ROW) {
        // int userId = sqlite3_column_int(res, 0); // Assuming the id is in the first column
        const unsigned char *username = sqlite3_column_text(res, 1); // Assuming username is in the second column
        const unsigned char *surname = sqlite3_column_text(res, 2);
        sprintf(temp_buff, "%s\n%s\n", username, surname); // Assuming surnmae is in the third column
        // send()
        // printf("User ID: %d, Username: %s\n", userId, username);
        // fflush(stdout);
    }
    sqlite3_finalize(res);
            // sendBuff = mx_strjoin(sendBuff, temp_buff);
        send(sockfd, temp_buff, strlen(temp_buff), 0);
    sqlite3_close(db);
        // free(temp_buff);
}

void mx_update_user_data(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    bzero(sql, 500);
    char *errmsg;
    sprintf(sql, "UPDATE USERS SET NAME='%s',"
            "SURNAME='%s',"
            "USERNAME='%s',"
            "DESCRIPTION='%s' WHERE ID=%d;",
            data[1], data[2], data[3], data[4], mx_atoi(data[5]));   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Update user data", st);
    sqlite3_close(db);
}

void mx_find_user(char **data, int sockfd) {
    char *sendBuff = NULL;

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
    sprintf(sql, "SELECT ID FROM USERS WHERE USERNAME='%s';", data[1]);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) sendBuff = mx_strjoin(sendBuff, "SUCCESS");
    else sendBuff = mx_strjoin(sendBuff, "FAIL");
    sqlite3_finalize(res);
    sqlite3_close(db);
    send(sockfd, sendBuff, strlen(sendBuff), 0);
    free(sendBuff);
}

bool mx_check_user(char **data) {
    char *encrypted_pass = encrypt_pass(mx_strdup(data[2]));

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
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
