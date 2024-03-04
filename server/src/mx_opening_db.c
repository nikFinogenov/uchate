#include "../inc/server.h"

sqlite3 *open_db(void) {
    sqlite3 *db;
    int status = sqlite3_open("server/data/McOk.db", &db);
    char* st = (status == 0) ? ST_OK : ST_NEOK;
    logger("Open database", st);
    return db;
}
