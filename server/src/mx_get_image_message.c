#include "../inc/server.h"

void mx_get_image_message(char **data, int sockfd) {
    int m_id = mx_atoi(data[1]);
    int uid = mx_atoi(data[2]);
    int dst = mx_atoi(data[3]);

    sqlite3 *db = open_db();
    char sql[200];
    bzero(sql, 200);
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

        unsigned int out_size = b64e_size(bytes) + 1;
        unsigned char *out_b64 = malloc( (sizeof(char) * out_size) );
        b64_encode(write_data, bytes, out_b64);
        free(write_data);

        int len_encoded = strlen((char *)out_b64);
        send(sockfd, &len_encoded, sizeof(int), 0);

        mx_send_all(&sockfd, out_b64, len_encoded);
        free(out_b64);
    }
    rc = sqlite3_finalize(pStmt); 
    sqlite3_close(db);
}
