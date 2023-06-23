#pragma once

void ErrMSG(const char *filename, const char *function, int line, const char *expr, const char *msg);
#define ASSERTFUNC(expr, msg) ((expr) ? ((void) 0) : ErrMSG(__FILE__, __func__, __LINE__, #expr, msg))