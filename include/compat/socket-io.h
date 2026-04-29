/*
 * Socket I/O wrappers for cross-platform compatibility.
 *
 * On POSIX, sockets are file descriptors and read()/write()/close() work.
 * On Windows, sockets are SOCKET handles and require recv()/send()/closesocket().
 *
 * These wrappers provide a consistent interface and map Winsock errors
 * to POSIX errno values that neatvnc expects.
 */

#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <errno.h>

static inline ssize_t socket_read(int fd, void* buf, size_t count) {
	int rc = recv((SOCKET)fd, (char*)buf, (int)count, 0);
	if (rc == SOCKET_ERROR) {
		int wsa_err = WSAGetLastError();
		if (wsa_err == WSAEWOULDBLOCK)
			errno = EAGAIN;
		else if (wsa_err == WSAECONNRESET || wsa_err == WSAECONNABORTED)
			errno = EPIPE;
		else
			errno = EIO;
		return -1;
	}
	return rc;
}

static inline int socket_close(int fd) {
	return closesocket((SOCKET)fd);
}

#else
#include <unistd.h>

static inline ssize_t socket_read(int fd, void* buf, size_t count) {
	return read(fd, buf, count);
}

static inline int socket_close(int fd) {
	return close(fd);
}

#endif
