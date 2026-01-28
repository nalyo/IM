#include "user_dao.h"
#include "db/db.h"
#include "utils/im_string.h"
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>

int dao_user_create(const char* username, const char* password)
{
    const char* sql = "INSERT INTO users(username,password) VALUES(?,?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_get(), sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("[DB ERROR] prepare failed: %s\n", sqlite3_errmsg(db_get()));
        return -1;
    }
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int dao_user_find_by_name(const char* username, user_record_t* out)
{
    const char* sql = "SELECT id,username,password FROM users WHERE username=?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_get(), sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        out->id = sqlite3_column_int(stmt, 0);
        SAFE_STRNCPY(out->username, sizeof(out->username), (const char*)sqlite3_column_text(stmt, 1));
        SAFE_STRNCPY(out->password, sizeof(out->password), (const char*)sqlite3_column_text(stmt, 2));
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);
    return -1;
}
