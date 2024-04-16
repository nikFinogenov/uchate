#include "server.h"

sqlite3 *open_db(void) {
    sqlite3 *db;
    int status = sqlite3_open("server/source/McOk.db", &db);
    char* st = (status == 0) ? ST_OK : ST_NEOK;
    // logger("Open database", st, "");
    return db;
}

int socket_init(int port) {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0) {
        perror("Creating socket error");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(port);    
    int flag = 1;  
    if (-1 == setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag))) {  
        perror("setsockopt fail");
    }
    int bind_status = bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(bind_status < 0) {
        perror("Bind error");
        exit(1);
    }
    return socketfd;
}

void db_init(void) {
    sqlite3 *db = open_db();
    char *err_msg;
    char *sql = "CREATE TABLE IF NOT EXISTS USERS(id INTEGER PRIMARY KEY AUTOINCREMENT,\
        username TEXT NOT NULL, password TEXT NOT NULL, name TEXT NOT NULL, surname TEXT NOT NULL, \
        description TEXT, status TEXT, date TEXT, token TEXT, profile_img BLOB);";

    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    // logger("Init table \"Users\"", st, err_msg);

    sql = "CREATE TABLE IF NOT EXISTS CHATS(id INTEGER PRIMARY KEY AUTOINCREMENT, \
        user1_id INTEGER NOT NULL, user2_id INTEGER NOT NULL);";
    exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    st = (exit == 0) ? ST_OK : ST_NEOK;
    // logger("Init table \"Chats\"", st, err_msg);

    sql = "CREATE TABLE IF NOT EXISTS MESSAGES(id INTEGER PRIMARY KEY AUTOINCREMENT, \
        chat_id INTEGER NOT NULL, text TEXT NOT NULL, type TEXT, date TEXT);";
    exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    st = (exit == 0) ? ST_OK : ST_NEOK;
    // logger("Init table \"Messages\"", st, err_msg);
    sqlite3_close(db);
}

void logger(char *proccess, char* status, char* errmsg) {
    FILE *fd = fopen("server.log", "a+t");
    if (fd == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t current_time = time(NULL);
    char current_time_str[20];
    strftime(current_time_str, sizeof(current_time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    char* err = (!mx_strcmp(status, ST_NEOK)) ? mx_strjoin(" -> ", errmsg) : "";
    fprintf(fd, "%s: %s -> %s%s\n", current_time_str, proccess, status, err);

    if (fclose(fd) != 0) {
        perror("Error closing log file");
    }
}

char *encrypt_pass(char *str) {
    const EVP_MD *md = EVP_sha256();
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char *hashed_password = (char*)malloc((SHA256_DIGEST_LENGTH * 2 + 1) * sizeof(char));

    if (!hashed_password || !mdctx) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, str, strlen(str));
    EVP_DigestFinal_ex(mdctx, hash, NULL);
    EVP_MD_CTX_free(mdctx);

    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hashed_password[i*2], "%02x", hash[i]);
    }
    hashed_password[SHA256_DIGEST_LENGTH * 2] = '\0';

    return hashed_password;
}

void mx_write_photo_to_bd(char *path, int id) {
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open image file\n");    
    }     
    fseek(fp, 0, SEEK_END);
    if (ferror(fp)) {
        fprintf(stderr, "fseek() failed\n");
        int r = fclose(fp);
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");          
        }    
    }  
    int flen = ftell(fp);
    if (flen == -1) {
        perror("error occurred");
        int r = fclose(fp);
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");
        }   
    }
    fseek(fp, 0, SEEK_SET);
    if (ferror(fp)) {
        fprintf(stderr, "fseek() failed\n");
        int r = fclose(fp);
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");
        }    
    }
    char *data = malloc(flen + 1);
    int size = fread(data, 1, flen, fp);
    if (ferror(fp)) {
        fprintf(stderr, "fread() failed\n");
        int r = fclose(fp);
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");
        }    
    }
    int r = fclose(fp);
    if (r == EOF) {
        fprintf(stderr, "Cannot close file handler\n");
    }    
    sqlite3 *db;
    int rc = sqlite3_open("server/source/McOk.db", &db);
    if (rc != 0) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    sqlite3_stmt *pStmt;
    char *sql = malloc((unsigned)flen + 500);
    memset(sql, 0, (unsigned)flen + 500);
    sprintf(sql, "UPDATE USERS SET profile_img = ? WHERE ID = '%d' ;", id);
    
    rc = sqlite3_prepare(db, sql, -1, &pStmt, 0);
    
    if (rc != 0)
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));

    sqlite3_bind_blob(pStmt, 1, data, size, SQLITE_STATIC);    
    rc = sqlite3_step(pStmt);
    if (rc != SQLITE_DONE) {
        printf("execution failed: %s", sqlite3_errmsg(db));
    }  
    sqlite3_finalize(pStmt);    
    sqlite3_close(db);
    free(sql);
    free(data);
}