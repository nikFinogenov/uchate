#include "../inc/server.h"

void mx_get_users_arr(char **data, int sockfd) {
    int user_id = mx_atoi(data[1]);

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    bzero(sql, 250);
    sprintf(sql, "SELECT addresser, destination FROM Messages\
            WHERE addresser=%u OR destination=%u ORDER BY time DESC;",
            user_id, user_id);

    unsigned int *uid_arr = NULL;
    int uid_arr_len = 0;
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        unsigned int addresser = (unsigned int)sqlite3_column_int64(res, 0);
        unsigned int destination = (unsigned int)sqlite3_column_int64(res, 1);
        if (addresser != (unsigned int)user_id && !mx_is_in_array(uid_arr, addresser)) {
            mx_insert_value(&uid_arr, addresser);
            uid_arr_len++;
        }
        if (destination != (unsigned int)user_id && !mx_is_in_array(uid_arr, destination)) {
            mx_insert_value(&uid_arr, destination);
            uid_arr_len++;
        }
            
    }
    sqlite3_finalize(res);
    sqlite3_close(db);

    if (!mx_is_in_array(uid_arr, 0)) {
        mx_insert_value(&uid_arr, 0);
        uid_arr_len++;
    }
        

    send(sockfd, &uid_arr_len, sizeof(int), 0);
    for (int i = 0; i < uid_arr_len; i++) {
        send(sockfd, &uid_arr[i], sizeof(unsigned int), 0);
    }
    free(uid_arr);
}
