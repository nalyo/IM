#include "friend_dao.h"
#include "db.h"
#include <stddef.h>
#include <stdio.h>
#include <sqlite3.h>

int friend_add_bidirectional(int user_id, int friend_id)
{
    sqlite3* db = db_get();
    char* errmsg = NULL;

    // 开始事务
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "BEGIN TRANSACTION failed: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    const char* sql_insert = "INSERT OR IGNORE INTO friends(user_id, friend_id) VALUES(?, ?)";
    sqlite3_stmt* stmt = NULL;

    // user -> friend
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL) != SQLITE_OK) goto fail;
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, friend_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) goto fail_stmt;
    sqlite3_finalize(stmt);

    // friend -> user
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL) != SQLITE_OK) goto fail;
    sqlite3_bind_int(stmt, 1, friend_id);
    sqlite3_bind_int(stmt, 2, user_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) goto fail_stmt;
    sqlite3_finalize(stmt);

    // 提交事务
    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "COMMIT failed: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    return 0;

fail_stmt:
    sqlite3_finalize(stmt);
fail:
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
    return -1;
}

int friend_delete_bidirectional(int user_id, int friend_id)
{
    sqlite3* db = db_get();
    char* errmsg = NULL;

    // 开始事务
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "BEGIN TRANSACTION failed: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    const char* sql_delete = "DELETE FROM friends WHERE user_id=? AND friend_id=?";
    sqlite3_stmt* stmt = NULL;

    // user -> friend
    if (sqlite3_prepare_v2(db, sql_delete, -1, &stmt, NULL) != SQLITE_OK) goto fail;
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, friend_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) goto fail_stmt;
    sqlite3_finalize(stmt);

    // friend -> user
    if (sqlite3_prepare_v2(db, sql_delete, -1, &stmt, NULL) != SQLITE_OK) goto fail;
    sqlite3_bind_int(stmt, 1, friend_id);
    sqlite3_bind_int(stmt, 2, user_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) goto fail_stmt;
    sqlite3_finalize(stmt);

    // 提交事务
    if (sqlite3_exec(db, "COMMIT;", NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "COMMIT failed: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    return 0;

fail_stmt:
    sqlite3_finalize(stmt);
fail:
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
    return -1;
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
        cb(name, 0, arg);  // online 状态由 client_manager 补
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
