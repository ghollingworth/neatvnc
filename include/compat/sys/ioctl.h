#pragma once
#ifdef _WIN32
/* Not needed on Windows */
#else
#include_next <sys/ioctl.h>
#endif
