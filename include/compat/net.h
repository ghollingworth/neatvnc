/*
 * Windows networking compatibility shims for neatvnc.
 *
 * Provides POSIX socket API equivalents on top of Winsock2 so that
 * server.c and stream/tcp.c compile with minimal source changes.
 */

#pragma once

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>

#pragma comment(lib, "ws2_32.lib")

/* --- errno mapping --- */
#ifndef EAGAIN
#define EAGAIN WSAEWOULDBLOCK
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

#ifndef EPIPE
#define EPIPE WSAECONNRESET
#endif

#ifndef EINPROGRESS
#define EINPROGRESS WSAEWOULDBLOCK
#endif

/* Map WSA errors to errno */
static inline int wsa_errno(void) {
	int e = WSAGetLastError();
	switch (e) {
	case WSAEWOULDBLOCK: return EAGAIN;
	case WSAECONNRESET:  return EPIPE;
	case WSAEINPROGRESS: return EINPROGRESS;
	default: return e;
	}
}

/* --- struct iovec / sendmsg emulation --- */

#ifndef IOV_MAX
#define IOV_MAX 64
#endif

struct iovec {
	void* iov_base;
	size_t iov_len;
};

struct msghdr {
	void* msg_name;
	int msg_namelen;
	struct iovec* msg_iov;
	size_t msg_iovlen;
	void* msg_control;
	size_t msg_controllen;
	int msg_flags;
};

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

static inline ssize_t sendmsg(int fd, const struct msghdr* msg, int flags)
{
	WSABUF* bufs = (WSABUF*)_alloca(sizeof(WSABUF) * msg->msg_iovlen);
	for (size_t i = 0; i < msg->msg_iovlen; i++) {
		bufs[i].buf = (char*)msg->msg_iov[i].iov_base;
		bufs[i].len = (ULONG)msg->msg_iov[i].iov_len;
	}

	DWORD sent = 0;
	int rc = WSASend((SOCKET)fd, bufs, (DWORD)msg->msg_iovlen, &sent,
	                 0, NULL, NULL);
	if (rc == SOCKET_ERROR) {
		errno = wsa_errno();
		return -1;
	}
	return (ssize_t)sent;
}

/* --- read/write/close on sockets --- */

static inline ssize_t sock_read(int fd, void* buf, size_t count) {
	int rc = recv((SOCKET)fd, (char*)buf, (int)count, 0);
	if (rc == SOCKET_ERROR) {
		errno = wsa_errno();
		return -1;
	}
	return rc;
}

static inline int sock_close(int fd) {
	return closesocket((SOCKET)fd);
}

/* Provide socket-compatible read/close.
 *
 * We do NOT use macros for close/read/write because they collide with
 * struct member names (e.g. self->impl->close, self->impl->read).
 * Instead, the source code should call these functions directly for
 * socket operations.
 *
 * For MinGW, we redirect close() to closesocket() and read() to recv()
 * only via the build system (-Dclose=sock_close etc.) or by patching
 * the few call sites.
 */

/* --- fcntl: set non-blocking --- */

#ifndef O_NONBLOCK
#define O_NONBLOCK 1
#endif
#ifndef F_SETFL
#define F_SETFL 4
#endif
#ifndef F_GETFL
#define F_GETFL 3
#endif

static inline int compat_fcntl(int fd, int cmd, ...) {
	if (cmd == F_SETFL) {
		u_long mode = 1;
		ioctlsocket((SOCKET)fd, FIONBIO, &mode);
	}
	return 0;
}

#ifndef fcntl
#define fcntl compat_fcntl
#endif

/* --- AF_UNIX stub (not needed on Windows, stub the header) --- */

#ifndef AF_UNIX
#define AF_UNIX 1
#endif

#ifndef _SOCKADDR_UN_DEFINED
#define _SOCKADDR_UN_DEFINED
struct sockaddr_un {
	short sun_family;
	char sun_path[108];
};
#endif

/* --- sys/param.h (for MIN/MAX) --- */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* --- ssize_t --- */
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef long long ssize_t;
#endif

/* --- unistd.h stub --- */
/* Covered by read/close above */

/* --- Winsock init helper --- */
static inline void compat_net_init(void) {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
}

#else /* !_WIN32 */

/* On POSIX, include the real headers */
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>

static inline void compat_net_init(void) {}

#endif /* _WIN32 */
