#pragma once
#ifdef _WIN32
/* struct iovec and sendmsg are defined in compat/net.h */
#else
#include_next <sys/uio.h>
#endif
