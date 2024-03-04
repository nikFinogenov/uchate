#include "../inc/server.h"


void mx_get_avatar(char **data, int sockfd) {
    sqlite3 *db = open_db();
    char sql[500];
    bzero(sql, 500);
    sprintf(sql, "SELECT PHOTO FROM USERS WHERE ID=%d;", mx_atoi(data[1]));
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

    unsigned int out_size = b64e_size(bytes) + 1;
    unsigned char *out_b64 = malloc( (sizeof(char) * out_size) );
    b64_encode(write_data, bytes, out_b64);
    free(write_data);

    int len_encoded = strlen((char *)out_b64);
    send(sockfd, &len_encoded, sizeof(int), 0);

    mx_send_all(&sockfd, out_b64, len_encoded);
    free(out_b64);

    sqlite3_finalize(pStmt);   
    sqlite3_close(db);
}
