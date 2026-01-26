#ifdef _WIN32
#include "im_thread.h"
#include <windows.h>

static im_exit_cb g_exit_cb = NULL;

int im_thread_create(im_thread_t* tid, im_thread_fn func, void* arg) {
    HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL);
    if (!h) return -1;
    *tid = h;
    return 0;
}

int im_thread_detach(im_thread_t tid) {
    CloseHandle((HANDLE)tid);
    return 0;
}

int im_thread_sleep_ms(unsigned int ms) {
    Sleep(ms);
    return 0;
}

static BOOL WINAPI console_handler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        if (g_exit_cb) g_exit_cb();
    }
    return TRUE;
}

int im_register_exit_cb(im_exit_cb cb) {
    g_exit_cb = cb;
    return SetConsoleCtrlHandler(console_handler, TRUE) ? 0 : -1;
}
#endif
