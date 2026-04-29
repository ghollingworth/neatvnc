/* threads.h shim for MinGW which doesn't have C11 threads.
 * neatvnc's logging.c uses thread_local for a per-thread log topic.
 */
#pragma once
#ifdef _WIN32
/* MinGW GCC supports __thread for thread-local storage */
#define thread_local __thread
#else
#include_next <threads.h>
#endif
