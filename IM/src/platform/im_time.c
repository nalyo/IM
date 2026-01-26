#include "im_time.h"
#include <time.h>
#include <stdio.h>

int im_format_time(int64_t timestamp, char* out, int out_size)
{
    if (!out || out_size <= 0) {
        return -1;
    }

    time_t t = (time_t)timestamp;
    struct tm tm_time;

#ifdef _WIN32
    if (localtime_s(&tm_time, &t) != 0) {
        return -1;
    }
#else
    if (localtime_r(&t, &tm_time) == NULL) {
        return -1;
    }
#endif

    if (strftime(out, out_size, "%Y-%m-%d %H:%M:%S", &tm_time) == 0) {
        return -1;
    }

    return 0;
}
