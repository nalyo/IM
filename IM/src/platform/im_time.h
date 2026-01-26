#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief Unix 时间戳（秒）转标准时间字符串
	 * @param timestamp  Unix timestamp (seconds)
	 * @param out        输出字符串缓冲区
	 * @param out_size   缓冲区大小
	 * @return 0 成功，-1 失败
	 */
	int im_format_time(int64_t timestamp, char* out, int out_size);

#ifdef __cplusplus
}
#endif
