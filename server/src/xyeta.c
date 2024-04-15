#include "server.h"

static void mx_send_all(int *socket, void *buffer, size_t length) {
    unsigned char *ptr = (unsigned char *)buffer;
    while (length > 0) {
        usleep(70000);
        int i = send(*socket, ptr, length, 0);
        ptr += i;
        length -= i;
    }
}

static void mx_recv_all(int *socket, unsigned char **buffer, size_t length) {
    unsigned char *ptr = *buffer;
    while (length > 0) {
        int i = recv(*socket, ptr, length, 0);
        ptr += i;
        length -= i;
    }
}

void mx_check_last_room(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int last_uid = 0;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    memset(sql, 0, 250);
    sprintf(sql, "SELECT addresser, destination FROM Messages\
            WHERE addresser=%d OR destination=%d ORDER BY rowid DESC LIMIT 1;",
            uid, uid);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) {
        int addresser = (int)sqlite3_column_int64(res, 0);
        int destination = (int)sqlite3_column_int64(res, 1);
        if (addresser != uid)
            last_uid = addresser;
        else
            last_uid = destination;
    }
    else {
        sqlite3_finalize(res);
        sqlite3_close(db);
        send(sockfd, &last_uid, sizeof(int), 0);
        return;
    }
    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, &last_uid, sizeof(int), 0);
}

void mx_update_theme(char **data) {
    sqlite3 *db = open_db();
    char sql[500];
    memset(sql, 0, 500);
    char *err_msg;
    sprintf(sql, "UPDATE USERS SET THEME=%d WHERE ID=%d;", mx_atoi(data[1]), mx_atoi(data[2]));
    sqlite3_exec(db, sql, NULL, 0, &err_msg);
    sqlite3_close(db);
}

void mx_update_language(char **data) {
    sqlite3 *db = open_db();
    char sql[256];
    memset(sql, 0, 256);
    char *errmsg;
    sprintf(sql, "UPDATE USERS SET LANGUAGE=%d WHERE ID=%d;", mx_atoi(data[1]), mx_atoi(data[2]));   
    int exit = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Update language", st, errmsg);
    sqlite3_close(db);
}

void mx_send_room_data(char **data, int sockfd) {
    char *sendBuff = NULL;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    memset(sql, 0, 250);
    sprintf(sql, "SELECT NAME, SURNAME, USERNAME FROM USERS\
            WHERE id=%u;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);

    sendBuff = mx_strjoin(sendBuff, (char *)sqlite3_column_text(res, 0));
    sendBuff = mx_strjoin(sendBuff, (char *)sqlite3_column_text(res, 1));
    sendBuff = mx_strjoin(sendBuff, "\n");

    char *tmp_preview = mx_strdup((char *)sqlite3_column_text(res, 2));
    tmp_preview = mx_strjoin("@", tmp_preview);
    sendBuff = mx_strjoin(sendBuff, tmp_preview);
    free(tmp_preview);

    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, sendBuff, mx_strlen(sendBuff), 0);
    free(sendBuff);
}

void mx_load_room(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int id = 0;

    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[250];
    memset(sql, 0, 250);
    sprintf(sql, "SELECT MAX(ID) FROM Messages\
            WHERE (addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d);",
            uid, dst, uid, dst);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (sqlite3_step(res) != SQLITE_DONE) {
        id = (int)sqlite3_column_int(res, 0);
        send(sockfd, &id, sizeof(int), 0);
    }
    else {
        sqlite3_finalize(res);
        sqlite3_close(db);
        send(sockfd, &id, sizeof(int), 0);
        return;
    }
    sqlite3_finalize(res);
    memset(sql, 0, 250);
    sprintf(sql, "SELECT id, addresser, Text, time FROM Messages\
            WHERE (addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d)\
            ORDER BY id;",
            uid, dst, uid, dst);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        char *message_text = (char *)sqlite3_column_text(res, 2);

        int cur_m_id = (int)sqlite3_column_int64(res, 0);

        char sendBuff[1024];
        memset(sendBuff, 0, 1024);
        sprintf(sendBuff, "%d\n%d\n%s\n%d", cur_m_id, 
            (int)sqlite3_column_int64(res, 1), message_text, (int)sqlite3_column_int64(res, 3));
        send(sockfd, sendBuff, 1024, 0);

        send(sockfd, &cur_m_id, sizeof(int), 0);
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
}
void mx_get_language(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[256];
    memset(sql, 0, 256);
    sprintf(sql, "SELECT LANGUAGE FROM USERS WHERE ID=%d;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);
    int language = (int)sqlite3_column_int(res, 0);
    send(sockfd, &language, sizeof(int), 0);
    sqlite3_finalize(res);
    sqlite3_close(db);
}
void mx_get_theme(char **data, int sockfd) {
    sqlite3 *db = open_db();
    sqlite3_stmt *res;
    char sql[500];
    memset(sql, 0, 500);
    sprintf(sql, "SELECT THEME FROM USERS WHERE ID=%d;", mx_atoi(data[1]));
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_step(res);
    int theme = (int)sqlite3_column_int(res, 0);
    sqlite3_finalize(res);
    sqlite3_close(db);

    send(sockfd, &theme, sizeof(int), 0);
}

void mx_get_avatar(char **data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];

    memset(sql, 0, 500);
    sprintf(sql, "SELECT profile_img FROM USERS WHERE ID=%d;", mx_atoi(data[1]));
    sqlite3_stmt *pStmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
    if (rc != 0 ) {
        fprintf(stderr, "Failed to prepare statement 11111\n");
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    } 
    rc = sqlite3_step(pStmt);

    int bytes = 0;
    if (rc == SQLITE_ROW)
        bytes = sqlite3_column_bytes(pStmt, 0);
    
    const void *blob_data = sqlite3_column_blob(pStmt, 0);
    if (blob_data == NULL) {    
        rc = sqlite3_finalize(pStmt);
        sqlite3_close(db);
        return;
    }
    unsigned char *write_data = malloc(bytes + 1);
    memcpy(write_data, blob_data, bytes);
    write_data[bytes] = '\0';

    unsigned int out_size = mx_b64e_size(bytes) + 1;
    unsigned char *out_b64 = malloc( (sizeof(char) * out_size) );
    mx_b64_encode(write_data, bytes, out_b64);
    free(write_data);

    int len_encoded = strlen((char *)out_b64);
    send(sockfd, &len_encoded, sizeof(int), 0);

    mx_send_all(&sockfd, out_b64, len_encoded);
    free(out_b64);

    sqlite3_finalize(pStmt);   
    sqlite3_close(db);
}
void mx_get_users_arr(char **data, int sockfd) {
    int user_id = mx_atoi(data[1]);

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    memset(sql, 0, 250);
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

void mx_get_image_message(char **data, int sockfd) {
    int m_id = mx_atoi(data[1]);
    int uid = mx_atoi(data[2]);
    int dst = mx_atoi(data[3]);

    sqlite3 *db = open_db();
    char sql[200];
    memset(sql, 0, 200);
    sprintf(sql, "SELECT Image FROM Messages WHERE id=%d AND\
            ((addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d));",
            m_id, dst, uid, dst, uid);
        
    sqlite3_stmt *pStmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
    
    if (rc != 0 ) {
        fprintf(stderr, "Failed to prepare statement 2222\n");
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));     
        rc = sqlite3_finalize(pStmt);
        sqlite3_close(db);
        return;
    }
    
    rc = sqlite3_step(pStmt);
    int bytes = 0;
    if (rc == SQLITE_ROW)
        bytes = sqlite3_column_bytes(pStmt, 0);

    send(sockfd, &bytes, sizeof(int), 0);

    if (bytes) {
        const void *blob_data = sqlite3_column_blob(pStmt, 0);
        if (blob_data == NULL) {    
            rc = sqlite3_finalize(pStmt);
            sqlite3_close(db);
            return;
        }
        unsigned char *write_data = malloc(bytes + 1);
        memcpy(write_data, blob_data, bytes);
        write_data[bytes] = '\0';

        unsigned int out_size = mx_b64e_size(bytes) + 1;
        unsigned char *out_b64 = malloc( (sizeof(char) * out_size) );
        mx_b64_encode(write_data, bytes, out_b64);
        free(write_data);

        int len_encoded = strlen((char *)out_b64);
        send(sockfd, &len_encoded, sizeof(int), 0);

        mx_send_all(&sockfd, out_b64, len_encoded);
        free(out_b64);
    }
    rc = sqlite3_finalize(pStmt); 
    sqlite3_close(db);
}
void message_img(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int m_id = mx_atoi(data[3]);

    char *eptr;
    unsigned int out_size = (unsigned)strtol(data[4], &eptr, 10);

    int len_encoded = mx_atoi(data[5]);
    
    unsigned char *encoded = malloc( (sizeof(char) * out_size) );
    memset(encoded, 0, out_size);

    mx_recv_all(&sockfd, &encoded, len_encoded);

    unsigned int flen = mx_b64d_size(len_encoded);
    unsigned char *decoded = malloc( (sizeof(char) * flen) );
    memset(decoded, 0, flen);
    flen = mx_b64_decode(encoded, len_encoded, decoded);
    free(encoded);
    
    sqlite3 *db = open_db();
    sqlite3_stmt *pStmt;

    char *sql = malloc(flen + 250);
    memset(sql, 0, flen + 250);
    sprintf(sql, "UPDATE Messages SET Image = ? WHERE id=%d AND\
            ((addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d));",
            m_id, uid, dst, uid, dst);
    
    int rc = sqlite3_prepare(db, sql, -1, &pStmt, 0);
    if (rc != 0)
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));

    sqlite3_bind_blob(pStmt, 1, decoded, flen, SQLITE_STATIC);
    rc = sqlite3_step(pStmt);
    if (rc != SQLITE_DONE)
        printf("execution failed: %s", sqlite3_errmsg(db));

    sqlite3_finalize(pStmt);  
    sqlite3_close(db);
    free(sql);
    free(decoded);
}
