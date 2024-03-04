#include "../inc/server.h"

bool mx_check_user(char **data) {
    char *encrypted_pass = encrypt_pass(mx_strdup(data[2]));

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    bzero(sql, 500);
    sprintf(sql, "SELECT PASSWORD FROM USERS WHERE PSEUDONIM='%s';", data[1]);
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
