#pragma once
#include <sqlite3.h>

int db_init(const char* db_name, const char* sqls[], int sql_count);
void db_close(void);

sqlite3* db_get(void);
