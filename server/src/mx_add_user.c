#include "../inc/server.h"

/*
    data - recv data from client
    data[0] - Operation
    data[1] - NAME
    data[2] - SURNAME
    data[3] - PSEUDONIM
    data[4] - PASSWORD
    data[5] - NULL
*/

void mx_add_user(char **data) {
    printf("add usere");
    char *encrypted_pass = mx_encryption(mx_strdup(data[4]));

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
    char *errmsg;
    sprintf(sql, "SELECT MAX(ID) FROM USERS;");
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);
    int id = (int)sqlite3_column_int(res, 0);
    sqlite3_finalize(res);
    id++;
    char *description = " ";
    sprintf(sql, 
            "INSERT INTO USERS (ID, NAME, SURENAME, PSEUDONIM, \
            DESCRIPTION, PASSWORD, LANGUAGE, THEME) VALUES('%d',\
            '%s','%s','%s','%s','%s','%d','%d');", 
            id, data[1], data[2], data[3], description, encrypted_pass, 0, 0);   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Add user", st);
    sqlite3_close(db);
    mx_write_photo_to_bd("server/data/standard_avatar.jpg", id);

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
    // char *encrypted_pass = mx_encryption(mx_strdup(data[4]));
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

void mx_get_user2(char **data, int sockfd) {
        char *sendBuff = NULL;
    if (mx_check_user(data)) {
        sendBuff = mx_strjoin(sendBuff, "SUCCESS\n");

        char temp_buff[1024];
        bzero(temp_buff, 1024);

        sqlite3 *db = open_db();
        sqlite3_stmt *res;
        char sql[500];
        bzero(sql, 500);
        sprintf(sql, "SELECT ID, NAME, SURENAME, PSEUDONIM, DESCRIPTION FROM USERS WHERE PSEUDONIM='%s';", data[1]);
        sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_step(res);
        sprintf(temp_buff, "%d\n%s\n%s\n%s\n%s\n",
                (int)sqlite3_column_int(res, 0),
                (char *)sqlite3_column_text(res, 1),
                (char *)sqlite3_column_text(res, 2),
                (char *)sqlite3_column_text(res, 3),
                (char *)sqlite3_column_text(res, 4));
        sqlite3_finalize(res);
        sqlite3_close(db);

        sendBuff = mx_strjoin(sendBuff, temp_buff);
        send(sockfd, sendBuff, strlen(sendBuff), 0);
    }
    else {
        sendBuff = strdup("FAIL");
        
        send(sockfd, sendBuff, strlen(sendBuff), 0);
    }
    free(sendBuff);
}
