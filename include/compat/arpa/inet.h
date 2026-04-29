/* arpa/inet.h shim for Windows — htons/ntohs/inet_pton are in winsock2.h */
#pragma once
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include_next <arpa/inet.h>
#endif
