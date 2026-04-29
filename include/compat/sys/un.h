#pragma once
#ifdef _WIN32
/* Unix sockets not used on Windows */
#ifndef AF_UNIX
#define AF_UNIX 1
#endif
#ifndef _SOCKADDR_UN_DEFINED
#define _SOCKADDR_UN_DEFINED
struct sockaddr_un { short sun_family; char sun_path[108]; };
#endif
#else
#include_next <sys/un.h>
#endif
