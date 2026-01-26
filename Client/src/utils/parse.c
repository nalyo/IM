#include "parse.h"
#include <string.h>
#include <ctype.h>

static const char* skip_space(const char* p)
{
    while (*p && isspace((unsigned char)*p))
        p++;
    return p;
}

int read_token(const char** p, char* out, int max_len)
{
    int i = 0;
    const char* s = skip_space(*p);
    if (*s == '\0') return -1;

    while (*s && !isspace((unsigned char)*s)) {
        if (i < max_len - 1)
            out[i++] = *s;
        s++;
    }

    out[i] = '\0';
    *p = s;
    return 0;
}

int read_rest(const char** p, char* out, int max_len)
{
    const char* s = skip_space(*p);
    int len = (int)strlen(s);
    if (len <= 0) return -1;
    if (len >= max_len) len = max_len - 1;
    memcpy(out, s, len);
    out[len] = '\0';
    return 0;
}
