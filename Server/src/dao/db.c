#include "db.h"
#include "im_err.h"
#include "platform/im_tools.h"
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

static sqlite3* g_db = NULL;

// 获取程序执行目录
static void get_exe_dir(char* out, size_t size)
{
#if defined(_WIN32) || defined(_WIN64)
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    char* last_backslash = strrchr(path, '\\');
    if (last_backslash) *last_backslash = '\0';
    SAFE_STRNCPY(out, size, path);
    out[size - 1] = '\0';
#else
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len == -1) {
        out[0] = '\0';
        return;
    }
    path[len] = '\0';
    char* last_slash = strrchr(path, '/');
    if (last_slash) *last_slash = '\0';
    strncpy(out, path, size);
    out[size - 1] = '\0';
#endif
}

int db_init(void)
{
    char exe_dir[1024];
    get_exe_dir(exe_dir, sizeof(exe_dir));

    char db_path[1024];
#if defined(_WIN32) || defined(_WIN64)
    snprintf(db_path, sizeof(db_path), "%s\\im_server.db", exe_dir);
#else
    snprintf(db_path, sizeof(db_path), "%s/im_server.db", exe_dir);
#endif

    if (sqlite3_open(db_path, &g_db) != SQLITE_OK) {
        printf("[DB ERROR] open failed: %s\n", sqlite3_errmsg(g_db));
        return IM_ERR_DB_FAILED;
    }

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

    printf("[DB] opened database: %s\n", db_path);
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
