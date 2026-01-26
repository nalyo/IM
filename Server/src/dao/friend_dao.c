#include "friend_dao.h"
#include "db.h"
#include <stddef.h>
#include <sqlite3.h>

int friend_add(int user_id, int friend_id)
{
    const char* sql =
        "INSERT OR IGNORE INTO friends(user_id, friend_id) VALUES(?, ?)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_get(), sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, friend_id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE ? 0 : -1;
}

int friend_delete(int user_id, int friend_id)
{
    const char* sql =
        "DELETE FROM friends WHERE user_id=? AND friend_id=?";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_get(), sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, friend_id);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return 0;
}

int friend_list(int user_id, friend_iter_cb cb, void* arg)
{
    const char* sql =
        "SELECT u.username "
        "FROM friends f "
        "JOIN users u ON f.friend_id = u.id "
        "WHERE f.user_id=?";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_get(), sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = (const char*)sqlite3_column_text(stmt, 0);
        cb(name, 0, arg);  // online ×´Ì¬ÓÉ client_manager ²¹
    }

    sqlite3_finalize(stmt);
    return 0;
}

int friend_is_friend(int user_id, int friend_id)
{
    const char* sql =
        "SELECT 1 FROM friends WHERE user_id=? AND friend_id=? LIMIT 1";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_get(), sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, friend_id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_ROW ? 1 : 0;
}
