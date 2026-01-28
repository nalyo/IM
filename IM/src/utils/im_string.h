#pragma once

#ifdef _WIN32
#define SAFE_STRNCPY(dest, destsz, src) \
    do { \
        strncpy_s((dest), (destsz), (src), _TRUNCATE); \
    } while(0)
#else
#include <string.h>
#define SAFE_STRNCPY(dest, destsz, src) \
    do { \
        size_t _len = strlen(src); \
        if (_len >= (destsz)) _len = (destsz) - 1; \
        memcpy((dest), (src), _len); \
        (dest)[_len] = '\0'; \
    } while(0)
#endif
