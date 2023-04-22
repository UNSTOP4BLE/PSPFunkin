#pragma once
 
#include <cstddef> 

namespace Mem {

extern int curusage;

void *pspf_malloc(size_t size);
void pspf_free(void *ptr);
}