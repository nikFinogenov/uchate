#include "server.h"

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

    unsigned int flen = b64d_size(len_encoded);
    unsigned char *decoded = malloc( (sizeof(char) * flen) );
    memset(decoded, 0, flen);
    flen = b64_decode(encoded, len_encoded, decoded);
    free(encoded);
    
    sqlite3 *db = open_db();
    sqlite3_stmt *pStmt;

    char *sql = malloc(flen + 250);
    bzero(sql, flen + 250);
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

void mx_edit_message(char **data) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int id = mx_atoi(data[3]);

    sqlite3 *db = open_db();
    char *err_msg = 0;
    char sql[300];
    bzero(sql, 300);
    sprintf(sql, "UPDATE Messages SET Text='%s' WHERE id=%d AND\
            ((addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d));", 
            data[4], id, dst, uid, dst, uid);
    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Edit message", st);
    sqlite3_close(db);
}

void mx_delete_message(char **data) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int id = mx_atoi(data[3]);

    sqlite3 *db = open_db();
    char *err_msg = 0;
    char sql[300];
    bzero(sql, 300);
    sprintf(sql, "DELETE FROM Messages WHERE id=%u AND\
            ((addresser=%u OR addresser=%u) AND (destination=%u OR destination=%u));", 
            id, dst, uid, dst, uid);
    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Delete sender message", st);

    sqlite3_stmt *res;
    bzero(sql, 300);
    sprintf(sql, "SELECT id FROM Messages WHERE id > %u AND\
            ((addresser=%u OR addresser=%u) AND (destination=%u OR destination=%u));", 
            id, dst, uid, dst, uid);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        unsigned int value = (unsigned int)sqlite3_column_int64(res, 0);
        sprintf(sql, "UPDATE Messages SET id=%u WHERE id=%u AND\
            ((addresser=%u OR addresser=%u) AND (destination=%u OR destination=%u));", 
            value - 1, value, dst, uid, dst, uid);
        exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
        st = (exit == 0) ? ST_OK : ST_NEOK;
        logger("Delete recipient message", st);
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
}
