#pragma once
#ifdef _WIN32
/* MinGW provides unistd.h — include it for pid_t, etc.
 * We use include_next to get MinGW's version, not recurse into ourselves.
 */
#include_next <unistd.h>
#include <io.h>
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef long long ssize_t;
#endif
#else
#include_next <unistd.h>
#endif
