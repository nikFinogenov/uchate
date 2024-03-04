#include "server.h"

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
