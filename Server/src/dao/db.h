#pragma once
#include <sqlite3.h>

int db_init(const char* path);
void db_close(void);

sqlite3* db_get(void);
