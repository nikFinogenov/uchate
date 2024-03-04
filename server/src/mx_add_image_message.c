#include "../inc/server.h"


void mx_add_image_message(char **data, int sockfd) {
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
