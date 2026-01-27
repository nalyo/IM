#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include "client_plugin.h"

/* 加载插件文件，支持 Windows/Linux/macOS */
int load_plugin(const char* path, client_app_t* app);
void load_all_plugins(const char* dir, client_app_t* app);

#endif
