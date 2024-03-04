#include "../inc/server.h"

static void search_by_pseudo(char *text, char **data, int sockfd) {
    char **search_split = mx_strsplit(text, ' ');
    int users_len = 0;
    unsigned int *users_arr = NULL;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    bzero(sql, 250);
    sprintf(sql, "SELECT ID FROM USERS\
            WHERE INSTR(PSEUDONIM, '%s') != 0 AND ID != %d;", search_split[0] + 1, mx_atoi(data[2]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        int uid = (int)sqlite3_column_int(res, 0);
        if (uid != mx_atoi(data[2]) && !mx_is_in_array(users_arr, uid)) {
            mx_insert_value(&users_arr, uid);
            users_len++;
        }
            
    }
    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, &users_len, sizeof(int), 0);

    if (users_len > 0) {
        for (int i = 0; i < users_len; i++)
            send(sockfd, &users_arr[i], sizeof(unsigned int), 0);
        
        free(users_arr);
    }

    mx_del_strarr(&search_split);
}

static void search_by_name(char *text, char **data, int sockfd) {
    char **search_split = mx_strsplit(text, ' ');
    int users_len = 0;
    unsigned int *users_arr = NULL;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    bzero(sql, 250);
    sprintf(sql, "SELECT ID FROM USERS\
            WHERE (INSTR(NAME, '%s') OR INSTR(SURENAME, '%s')) AND ID != %u;",
            search_split[0], search_split[1], mx_atoi(data[2]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        int uid = (int)sqlite3_column_int(res, 0);
        if (uid != mx_atoi(data[2]) && !mx_is_in_array(users_arr, uid)) {
            mx_insert_value(&users_arr, uid);
            users_len++;
        }
    }
    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, &users_len, sizeof(int), 0);

    if (users_len > 0) {
        for (int i = 0; i < users_len; i++)
            send(sockfd, &users_arr[i], sizeof(unsigned int), 0);
        
        free(users_arr);
    }

    mx_del_strarr(&search_split);
}

void mx_search_init(char **data, int sockfd) {
    char *search_text = mx_strdup(data[1]);

    if (search_text[0] == '@') {
        if (mx_strlen(search_text) == 1) {
            int users_len = 0;
            send(sockfd, &users_len, sizeof(int), 0);
            free(search_text);
            return;
        }
        search_by_pseudo(search_text, data, sockfd);
    }
    else
        search_by_name(search_text, data, sockfd);

    free(search_text);
}
