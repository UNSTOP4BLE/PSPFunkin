#pragma once

void debugLog(const char *format, ...);
void debugLogClear(void);
void ErrMSG(const char *filename, const char *function, int line, const char *expr, const char *msg);
#define ASSERTFUNC(expr, msg) ((expr) ? ((void) 0) : ErrMSG(__FILE__, __func__, __LINE__, #expr, msg))
#define PRINT_ALLOC() debugLog("%p alloc %s", this, __func__)
#define PTR_PRINT_ALLOC(ptr) debugLog("%p alloc %s", ptr, __func__)
#define PRINT_FREE() debugLog("%p free %s", this, __func__)

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))