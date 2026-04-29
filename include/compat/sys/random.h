#pragma once
#ifdef _WIN32
/* getrandom() provided by compat/platform.h via BCryptGenRandom */
#else
#include_next <sys/random.h>
#endif
