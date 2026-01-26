#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	typedef void* im_thread_t;
	typedef void* (*im_thread_fn)(void*);

	/* -------------------- 线程接口 -------------------- */
	int im_thread_create(im_thread_t* tid, im_thread_fn func, void* arg);
	int im_thread_detach(im_thread_t tid);
	int im_thread_sleep_ms(unsigned int ms);

	/* -------------------- 信号 / 退出处理 -------------------- */

	/**
	 * 注册全局退出回调
	 * 当接收到 Ctrl+C / SIGINT / 控制台关闭时触发
	 */
	typedef void (*im_exit_cb)(void);

	int im_register_exit_cb(im_exit_cb cb);

#ifdef _WIN32
#include <windows.h>
	typedef CRITICAL_SECTION im_mutex_t;
#define im_mutex_init(m) InitializeCriticalSection(m)
#define im_mutex_lock(m) EnterCriticalSection(m)
#define im_mutex_unlock(m) LeaveCriticalSection(m)
#define im_mutex_destroy(m) DeleteCriticalSection(m)
#else
#include <pthread.h>
	typedef pthread_mutex_t im_mutex_t;
#define im_mutex_init(m) pthread_mutex_init(m, NULL)
#define im_mutex_lock(m) pthread_mutex_lock(m)
#define im_mutex_unlock(m) pthread_mutex_unlock(m)
#define im_mutex_destroy(m) pthread_mutex_destroy(m)
#endif

#ifdef __cplusplus
}
#endif
