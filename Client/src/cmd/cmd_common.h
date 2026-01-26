#pragma once
#include "client_app.h"

typedef int (*cmd_handler_fn)(client_app_t* app, const char* args);

typedef struct {
    const char* name;
    cmd_handler_fn  handler;
    uint16_t sub_cmd;
} cmd_entry_t;
