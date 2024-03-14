#include "server.h"

sqlite3 *open_db(void) {
    sqlite3 *db;
    int status = sqlite3_open("server/data/McOk.db", &db);
    char* st = (status == 0) ? ST_OK : ST_NEOK;
    logger("Open database", st);
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
    char *sql = "CREATE TABLE IF NOT EXISTS USERS("
        "ID          INTEGER,"
        "NAME        TEXT NOT NULL, "
        "SURENAME    TEXT NOT NULL, "
        "PSEUDONIM   TEXT NOT NULL, "
        "DESCRIPTION TEXT NOT NULL, "
        "PASSWORD    TEXT NOT NULL, "
        "LANGUAGE    INTEGER, "
        "THEME       INTEGER, "
        "PHOTO       BLOB);";
    int exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    char* st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Create table \"Users\"", st);
    
    sql = "CREATE TABLE IF NOT EXISTS Messages(id BIGINT, \
           addresser BIGINT, destination BIGINT, Text TEXT, \
           Image BLOB, time BIGINT);";
    exit = sqlite3_exec(db, sql, NULL, 0, &err_msg);
    st = (exit == 0) ? ST_OK : ST_NEOK;
    logger("Create table \"Messages\"", st);
    sqlite3_close(db);
}

void logger(char *proccess, char* status) {
    FILE *fd = fopen("server.log", "a+t");
    if (fd == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t current_time = time(NULL);
    char current_time_str[20];
    strftime(current_time_str, sizeof(current_time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // char* input_color = (!mx_strcmp(status, ST_OK)) ? GREEN : RED;
    fprintf(fd, "%s: %s -> %s\n", current_time_str, proccess, status);

    if (fclose(fd) != 0) {
        perror("Error closing log file");
    }
}

char *encrypt_pass(char *str) {
    int len = mx_strlen(str);
    for (int i = 0; str[i]; i++) {
        if (str[i] == str[0])
            str[i] = str[i] + 1;
        else if (str[i] == str[len - 1])
            str[i] = str[i] + 3;
        else if (i % 2 == 0) 
            str[i] = str[i] + 2;
        else {
            str[i] = str[i] - 2;
        }
    }
    return str;
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
    int rc = sqlite3_open("server/data/McOk.db", &db);
    if (rc != 0) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    sqlite3_stmt *pStmt;
    char *sql = malloc((unsigned)flen + 500);
    memset(sql, 0, (unsigned)flen + 500);
    sprintf(sql, "UPDATE USERS SET PHOTO = ? WHERE ID = '%d' ;", id);
    
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

static void search_by_pseudo(char *text, char **data, int sockfd) {
    char **search_split = mx_strsplit(text, ' ');
    int users_len = 0;
    unsigned int *users_arr = NULL;

    sqlite3 *db = open_db();
    sqlite3_stmt *res = NULL;
    char sql[250];
    memset(sql, 0, 250);
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
    memset(sql, 0, 250);
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

/*
    data - recv data from client
    data[0] - Operation
    data[1] - Pseudonim/Username
    data[2] - Password
    data[3] - NULL
*/
void mx_authorization(char **data, int sockfd) {
    char *sendBuff = NULL;
    if (mx_check_user(data)) {
        sendBuff = mx_strjoin(sendBuff, "SUCCESS\n");

        char temp_buff[1024];
        memset(temp_buff, 0, 1024);

        sqlite3 *db = open_db();
        sqlite3_stmt *res;
        char sql[500];
        memset(sql, 0, 500);
        sprintf(sql, "SELECT ID, NAME, SURENAME, PSEUDONIM, DESCRIPTION FROM USERS WHERE PSEUDONIM='%s';", data[1]);
        sqlite3_prepare_v2(db, sql, -1, &res, 0);
        sqlite3_step(res);
        sprintf(temp_buff, "%d\n%s\n%s\n%s\n%s\n",
                (int)sqlite3_column_int(res, 0),
                (char *)sqlite3_column_text(res, 1),
                (char *)sqlite3_column_text(res, 2),
                (char *)sqlite3_column_text(res, 3),
                (char *)sqlite3_column_text(res, 4));
        sqlite3_finalize(res);
        sqlite3_close(db);

        sendBuff = mx_strjoin(sendBuff, temp_buff);
        send(sockfd, sendBuff, strlen(sendBuff), 0);
    }
    else {
        sendBuff = strdup("FAIL");
        
        send(sockfd, sendBuff, strlen(sendBuff), 0);
    }
    free(sendBuff);
}
void mx_load_messages(char **data, int sockfd) {
    int uid = mx_atoi(data[1]);
    int dst = mx_atoi(data[2]);
    int max_msg_id = mx_atoi(data[3]);
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
            WHERE (addresser=%d OR addresser=%d) AND (destination=%d OR destination=%d) AND id > %d\
            ORDER BY id;",
            uid, dst, uid, dst, max_msg_id);
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    while (sqlite3_step(res) != SQLITE_DONE) {
        char *message_text = (char *)sqlite3_column_text(res, 2);

        int m_id = (int)sqlite3_column_int64(res, 0);

        char sendBuff[1024];
        memset(sendBuff, 0, 1024);
        sprintf(sendBuff, "%d\n%d\n%s\n%d", m_id, 
            (int)sqlite3_column_int64(res, 1), message_text, (int)sqlite3_column_int64(res, 3));
        send(sockfd, sendBuff, 1024, 0);

        send(sockfd, &m_id, sizeof(int), 0);
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
}