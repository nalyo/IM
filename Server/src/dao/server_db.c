#include "server_db.h"
#include "db/db.h"

int server_db_init(void)
{
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
    return db_init("im_server.db", sqls, sizeof(sqls) / sizeof(sqls[0]));
}
