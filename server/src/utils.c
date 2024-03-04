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
    bzero((char *)&serv_addr, sizeof(serv_addr));
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
