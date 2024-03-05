#include "server/inc/server.h"

#include <stdio.h>
#include <sqlite3.h>

int init() {
    sqlite3 *db;
    char *errMsg = 0;

    // Open or create the database
    int rc = sqlite3_open("XYI.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    // SQL statement to create the Users table
    const char *createUsersTableSQL = "CREATE TABLE Users ("
                                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "username TEXT NOT NULL,"
                                      "password TEXT NOT NULL,"
                                      "name TEXT NOT NULL,"
                                      "description TEXT,"
                                      "status TEXT,"
                                      "date TEXT,"
                                      "token TEXT,"
                                      "profile_img BLOB"
                                      ");";

    // Execute the SQL statement for creating Users table
    rc = sqlite3_exec(db, createUsersTableSQL, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (Users table): %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    } else {
        fprintf(stdout, "Users table created successfully\n");
    }

    // SQL statement to create the Chat table
    const char *createChatTableSQL = "CREATE TABLE Chat ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "user1_id INTEGER NOT NULL,"
                                     "user2_id INTEGER NOT NULL,"
                                     "creation_date TEXT NOT NULL"
                                     ");";

    // Execute the SQL statement for creating Chat table
    rc = sqlite3_exec(db, createChatTableSQL, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (Chat table): %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    } else {
        fprintf(stdout, "Chat table created successfully\n");
    }

    // SQL statement to create the Messages table
    const char *createMessagesTableSQL = "CREATE TABLE Messages ("
                                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                         "chat_id INTEGER NOT NULL,"
                                         "text TEXT NOT NULL,"
                                         "type TEXT,"
                                         "status TEXT"
                                         ");";

    // Execute the SQL statement for creating Messages table
    rc = sqlite3_exec(db, createMessagesTableSQL, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (Messages table): %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    } else {
        fprintf(stdout, "Messages table created successfully\n");
    }

    // Close the database
    sqlite3_close(db);

    return 0;
}

int addUser(sqlite3 *db, const char *username, const char *password, const char *name,
            const char *description, const char *status, const char *date,
            const char *token, const char *profile_img) {
    char *errMsg = 0;

    // SQL statement to insert a user into the Users table
    const char *sql = "INSERT INTO Users (username, password, name, description, status, date, token, profile_img) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    // SQLite prepared statement
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, status, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, token, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, profile_img, -1, SQLITE_STATIC);

    // Execute the statement
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL execution error: %s\n", sqlite3_errmsg(db));
        return rc;
    } else {
        fprintf(stdout, "User added successfully\n");
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    return 0;
}

int main() {
    init();
    sqlite3 *db;
    char *errMsg = 0;

    // Open or create the database
    int rc = sqlite3_open("XYI.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    // Example user data
    const char *username = "john_doe";
    const char *password = "password123";
    const char *name = "John Doe";
    const char *description = "A sample user";
    const char *status = "active";
    const char *date = "2024-03-05";
    const char *token = "abcd1234";
    const char *profile_img = "path/to/profile_img.jpg";

    // Add user to the database
    rc = addUser(db, username, password, name, description, status, date, token, profile_img);

    // Close the database
    sqlite3_close(db);

    return 0;
}