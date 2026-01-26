#pragma once

typedef struct {
    int id;
    char username[32];
    char password[64];
} user_record_t;

int dao_user_init(const char* db_path);
void dao_user_close(void);

int dao_user_create(const char* username, const char* password);
int dao_user_find_by_name(const char* username, user_record_t* out);
