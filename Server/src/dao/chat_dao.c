#include "chat_dao.h"
#include "db.h"
#include "platform/im_tools.h"
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
int message_save(int from_id, int to_id, const char* msg)
{
    const char* sql =
        "INSERT INTO messages(from_id, to_id, content, timestamp) VALUES(?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_get(), sql, -1, &stmt, NULL);

    int timestamp = (int)time(NULL);  // 当前 Unix 时间戳
    sqlite3_bind_int(stmt, 1, from_id);
    sqlite3_bind_int(stmt, 2, to_id);
    sqlite3_bind_text(stmt, 3, msg, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, timestamp);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE ? 0 : -1;
}

/* 获取两个用户的聊天记录，按时间升序（或降序可修改）返回最多 max_count 条 */
/**
 * 获取两个用户之间的聊天历史
 *
 * @param user1      第一个用户名
 * @param user2      第二个用户名
 * @param out_msgs   输出数组，存放聊天记录
 * @param max_count  最大返回条数
 * @return           实际返回条数
 */
int message_get_history(const char* user1,
    const char* user2,
    im_chat_msg_t* out_msgs,
    int max_count)
{
    if (!user1 || !user2 || !out_msgs || max_count <= 0)
        return 0;

    sqlite3* db = db_get();
    if (!db) return 0;

    /* ------------------ 第一步：查用户名对应 ID ------------------ */
    int user1_id = -1, user2_id = -1;
    sqlite3_stmt* stmt = NULL;
    const char* sql_id = "SELECT id FROM users WHERE username=?";

    // user1
    if (sqlite3_prepare_v2(db, sql_id, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[DB ERROR] prepare failed: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(stmt, 1, user1, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
        user1_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    // user2
    if (sqlite3_prepare_v2(db, sql_id, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[DB ERROR] prepare failed: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    sqlite3_bind_text(stmt, 1, user2, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
        user2_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (user1_id < 0 || user2_id < 0)
        return 0; // 用户不存在

    /* ------------------ 第二步：查询聊天记录 ------------------ */
    const char* sql =
        "SELECT m.from_id, m.to_id, m.content, m.timestamp, "
        "       u_from.username AS from_user, u_to.username AS to_user "
        "FROM messages m "
        "JOIN users u_from ON m.from_id = u_from.id "
        "JOIN users u_to   ON m.to_id   = u_to.id "
        "WHERE (m.from_id=? AND m.to_id=?) "
        "   OR (m.from_id=? AND m.to_id=?) "
        "ORDER BY m.timestamp DESC "
        "LIMIT ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[DB ERROR] prepare failed: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user1_id);
    sqlite3_bind_int(stmt, 2, user2_id);
    sqlite3_bind_int(stmt, 3, user2_id);
    sqlite3_bind_int(stmt, 4, user1_id);
    sqlite3_bind_int(stmt, 5, max_count);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        const char* from_user = (const char*)sqlite3_column_text(stmt, 4);
        const char* to_user = (const char*)sqlite3_column_text(stmt, 5);
        const char* content = (const char*)sqlite3_column_text(stmt, 2);
        int timestamp = sqlite3_column_int(stmt, 3);

        SAFE_STRNCPY(out_msgs[count].from, sizeof(out_msgs[count].from), from_user);
        SAFE_STRNCPY(out_msgs[count].to, sizeof(out_msgs[count].to), to_user);
        SAFE_STRNCPY(out_msgs[count].message, sizeof(out_msgs[count].message), content);
        out_msgs[count].timestamp = timestamp;

        count++;
    }

    sqlite3_finalize(stmt);

    /* ------------------ 第三步：按时间升序排列 ------------------ */
    for (int i = 0; i < count / 2; ++i) {
        im_chat_msg_t tmp = out_msgs[i];
        out_msgs[i] = out_msgs[count - 1 - i];
        out_msgs[count - 1 - i] = tmp;
    }

    return count;
}