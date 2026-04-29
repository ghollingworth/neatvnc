#pragma once
#ifdef _WIN32
/* mmap not used on Windows */
#else
#include_next <sys/mman.h>
#endif
