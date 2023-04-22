#include "memory.h"                                                                                                                                                                                                                                                          
#include <cstdlib>       
#include <cstdio>   
#include <cstring>   

namespace Mem {

int curusage;

void *pspf_malloc(size_t size)
{
    curusage += (int)size;
    return malloc(size);
}

void pspf_free(void *ptr)
{
    curusage -= (int)sizeof(ptr);
    return free(ptr);
}

}

/* GCC builtins */

extern "C" void *__builtin_new(size_t size) {
    return Mem::pspf_malloc(size);
}

extern "C" void __builtin_delete(void *ptr) {
    Mem::pspf_free(ptr);
}

/* Default new/delete operators */

void *operator new(size_t size) noexcept {
    return Mem::pspf_malloc(size);
}

void *operator new[](size_t size) noexcept {
    return Mem::pspf_malloc(size);
}

void operator delete(void *ptr) noexcept {
    Mem::pspf_free(ptr);
}

void operator delete[](void *ptr) noexcept {
    Mem::pspf_free(ptr);
}

// https://en.cppreference.com/w/cpp/memory/new/operator_delete
void operator delete(void *ptr, size_t size) noexcept {
    Mem::pspf_free(ptr);
}

void operator delete[](void *ptr, size_t size) noexcept {
    Mem::pspf_free(ptr);
}