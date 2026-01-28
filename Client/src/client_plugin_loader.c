#include "client_plugin_loader.h"
#include "client_app.h"
#include "log.h"
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
typedef HMODULE lib_handle;
#define load_library(path) LoadLibraryA(path)
#define get_symbol(handle, name) GetProcAddress(handle, name)
#define close_library(handle) FreeLibrary(handle)
#else
#include <dlfcn.h>
typedef void* lib_handle;
#define load_library(path) dlopen(path, RTLD_NOW)
#define get_symbol(handle, name) dlsym(handle, name)
#define close_library(handle) dlclose(handle)
#endif

typedef client_plugin_t* (*plugin_init_func)();

int load_plugin(const char* path, client_app_t* app) {
    lib_handle handle = load_library(path);
    if (!handle) {
#if defined(_WIN32) || defined(_WIN64)
        log_error("LoadLibrary failed");
#else
        log_error("dlopen failed: %s", dlerror());
#endif
        return -1;
    }

    plugin_init_func init = (plugin_init_func)get_symbol(handle, "plugin_init");
    if (!init) {
        log_error("Cannot find symbol 'plugin_init'");
        close_library(handle);
        return -1;
    }

    client_plugin_t* plugin = init();
    if (!plugin) {
        log_error("plugin_init returned NULL");
        close_library(handle);
        return -1;
    }

    client_plugin_register(&app->g_plugin, plugin);
    log_debug("Loaded plugin: %s", plugin->name);

    return 0;
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

void load_all_plugins(const char* subdir, client_app_t* app) {
#if defined(_WIN32) || defined(_WIN64)
    char exe_path[MAX_PATH];
    // 获取程序（EXE）路径
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);

    // 去掉可执行文件名，保留目录
    char* last_backslash = strrchr(exe_path, '\\');
    if (last_backslash) {
        *last_backslash = '\0'; // 截断到目录
    }

    // 拼接插件目录
    char plugin_dir[MAX_PATH];
    snprintf(plugin_dir, MAX_PATH, "%s\\%s", exe_path, subdir);

    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*.dll", plugin_dir);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(search_path, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        log_debug("No plugins found in: %s", plugin_dir);
        return;
    }

    do {
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s\\%s", plugin_dir, fd.cFileName);
        load_plugin(full_path, app);
    } while (FindNextFileA(h, &fd));

    FindClose(h);
#else
    // Linux 部分可以用 readlink 获取 /proc/self/exe
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) return;
    exe_path[len] = '\0';

    char* last_slash = strrchr(exe_path, '/');
    if (last_slash) *last_slash = '\0';

    char plugin_dir[1024];
    snprintf(plugin_dir, sizeof(plugin_dir), "%s/%s", exe_path, subdir);

    DIR* dp = opendir(plugin_dir);
    if (!dp) return;

    struct dirent* entry;
    while ((entry = readdir(dp)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len > 3 && strcmp(entry->d_name + len - 3, ".so") == 0) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", plugin_dir, entry->d_name);
            load_plugin(full_path, app);
        }
    }
    closedir(dp);
#endif
}