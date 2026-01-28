#include "db.h"
#include "im_err.h"
#include "utils/im_string.h"
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

int db_init(const char* db_name, const char* sqls[], int sql_count)
{
    char exe_dir[1024];
    get_exe_dir(exe_dir, sizeof(exe_dir));

    char db_path[1024];
#if defined(_WIN32) || defined(_WIN64)
    snprintf(db_path, sizeof(db_path), "%s\\%s", exe_dir, db_name);
#else
    snprintf(db_path, sizeof(db_path), "%s/%s", exe_dir, db_name);
#endif

    if (sqlite3_open(db_path, &g_db) != SQLITE_OK) {
        printf("[DB ERROR] open failed: %s\n", sqlite3_errmsg(g_db));
        return IM_ERR_DB_FAILED;
    }

    char* errmsg = NULL;
    for (int i = 0; i < sql_count; ++i) {
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
