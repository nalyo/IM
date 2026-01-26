#ifndef _WIN32
#include "im_thread.h"
#include <pthread.h>
#include <unistd.h>

static im_exit_cb g_exit_cb = NULL;

int im_thread_create(im_thread_t* tid, im_thread_fn func, void* arg) {
    pthread_t t;
    if (pthread_create(&t, NULL, func, arg) != 0) return -1;
    *tid = (im_thread_t)(uintptr_t)t;
    return 0;
}

int im_thread_detach(im_thread_t tid) {
    pthread_t t = (pthread_t)(uintptr_t)tid;
    return pthread_detach(t);
}

int im_thread_sleep_ms(unsigned int ms) {
    usleep(ms * 1000);
    return 0;
}

static void sigint_handler(int sig) {
    (void)sig;
    if (g_exit_cb) g_exit_cb();
}

int im_register_exit_cb(im_exit_cb cb) {
    g_exit_cb = cb;
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    return sigaction(SIGINT, &sa, NULL) == 0 ? 0 : -1;
}

#endif
