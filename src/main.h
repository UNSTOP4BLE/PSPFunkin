#pragma once

extern char errstr[256];
void ErrMSG(const char *filename, const char *function, int line, const char *expr);
#define ASSERTFUNC(expr) ((expr) ? ((void) 0) : ErrMSG(__FILE__, __func__, __LINE__, #expr))
double getDT(void);