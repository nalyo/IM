#include "db.h"
#include "im_err.h"
#include <stdio.h>

static sqlite3* g_db = NULL;

int db_init(const char* path)
{
    if (sqlite3_open(path, &g_db) != SQLITE_OK) {
        printf("[DB ERROR] open failed: %s\n", sqlite3_errmsg(g_db));
        return IM_ERR_DB_FAILED;
    }

    // 所有表和索引的 SQL 指令
    const char* sqls[] = {
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP);",

        "CREATE TABLE IF NOT EXISTS friends ("
        "user_id INTEGER NOT NULL, "
        "friend_id INTEGER NOT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "PRIMARY KEY(user_id, friend_id));",

        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "from_id INTEGER NOT NULL, "
        "to_id INTEGER NOT NULL, "
        "content TEXT NOT NULL, "
        "timestamp INTEGER NOT NULL DEFAULT 0);",

        "CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);",
        "CREATE INDEX IF NOT EXISTS idx_friends_user ON friends(user_id);"
    };

    char* errmsg = NULL;
    for (int i = 0; i < sizeof(sqls) / sizeof(sqls[0]); ++i) {
        if (sqlite3_exec(g_db, sqls[i], NULL, NULL, &errmsg) != SQLITE_OK) {
            printf("[DB ERROR] exec failed: %s\nSQL: %s\n", errmsg, sqls[i]);
            sqlite3_free(errmsg);
            return IM_ERR_DB_FAILED;
        }
    }

    return IM_ERR_OK;
}

void db_close(void)
{
    if (g_db) {
        sqlite3_close(g_db);
        g_db = NULL;
    }
}

sqlite3* db_get(void)
{
    return g_db;
}

