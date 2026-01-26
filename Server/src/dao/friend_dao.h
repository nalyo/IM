#pragma once

int friend_add(int user_id, int friend_id);
int friend_delete(int user_id, int friend_id);

/* ±éÀúºÃÓÑ */
typedef void (*friend_iter_cb)(const char* name, int online, void* arg);
int friend_list(int user_id, friend_iter_cb cb, void* arg);
