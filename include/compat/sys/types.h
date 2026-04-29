#pragma once
#ifdef _WIN32
/* MinGW has sys/types.h — pass through to it */
#include_next <sys/types.h>
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef long long ssize_t;
#endif
#else
#include_next <sys/types.h>
#endif
