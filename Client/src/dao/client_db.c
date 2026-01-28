#include "client_db.h"
#include "db/db.h"

int client_db_init(void)
{
    const char* sqls[] = {
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "from_user TEXT NOT NULL,"
        "to_user TEXT NOT NULL,"
        "content TEXT NOT NULL,"
        "timestamp INTEGER NOT NULL);",
        "CREATE INDEX IF NOT EXISTS idx_messages_to_user ON messages(to_user);",
        "CREATE INDEX IF NOT EXISTS idx_messages_from_user ON messages(from_user);"
    };
    return db_init("im_server.db", sizeof(sqls) / sizeof(sqls[0]));
}
