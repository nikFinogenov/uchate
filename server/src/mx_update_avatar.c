#include "../inc/server.h"

void mx_update_avatar(char **data, int sockfd) {
    int len_encoded = 0;
    recv(sockfd, &len_encoded, sizeof(int), 0);

    unsigned char *encoded = malloc(len_encoded);
    mx_memset(encoded, 0, len_encoded);
    mx_recv_all(&sockfd, &encoded, len_encoded);

    unsigned int flen = b64d_size(len_encoded);
    unsigned char *decoded = malloc( (sizeof(char) * flen) );
    memset(decoded, 0, flen);
    flen = b64_decode(encoded, len_encoded, decoded);
    free(encoded);

    sqlite3 *db = open_db();
    sqlite3_stmt *pStmt;
    char *sql = malloc(flen + 40);
    bzero(sql, flen + 40);
    sprintf(sql, "UPDATE USERS SET PHOTO = ? WHERE ID=%d;", mx_atoi(data[1]));
    
    int rc = sqlite3_prepare(db, sql, -1, &pStmt, 0);
    
    if (rc != 0) fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
    sqlite3_bind_blob(pStmt, 1, decoded, flen, SQLITE_STATIC);
    rc = sqlite3_step(pStmt);
    if (rc != SQLITE_DONE)
        printf("execution failed: %s", sqlite3_errmsg(db));
    sqlite3_finalize(pStmt);    
    sqlite3_close(db);
    free(sql);
    free(decoded);
}
