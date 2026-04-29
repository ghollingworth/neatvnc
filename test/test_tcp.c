/*
 * Test TCP networking compatibility on both POSIX and Windows.
 *
 * Tests:
 * 1. Creating a listening TCP socket on localhost
 * 2. Connecting to it
 * 3. Sending data via sendmsg (scatter-gather)
 * 4. Receiving data
 * 5. Non-blocking I/O
 *
 * Uses the compat/net.h header to abstract platform differences.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "compat/net.h"

static int test_listen_accept_send(void)
{
	printf("  test_listen_accept_send: ");

	compat_net_init();

	/* Create listener */
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	assert(listener >= 0);

	int one = 1;
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
	           (const char*)&one, sizeof(one));

	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = 0; /* random port */

	assert(bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == 0);

	int addrlen = sizeof(addr);
	assert(getsockname(listener, (struct sockaddr*)&addr, &addrlen) == 0);
	uint16_t port = ntohs(addr.sin_port);
	assert(port > 0);

	assert(listen(listener, 1) == 0);

	/* Connect a client */
	int client = socket(AF_INET, SOCK_STREAM, 0);
	assert(client >= 0);

	assert(connect(client, (struct sockaddr*)&addr, sizeof(addr)) == 0);

	/* Accept on server side */
	int server_fd = accept(listener, NULL, NULL);
	assert(server_fd >= 0);

	/* Send data via sendmsg (scatter-gather) */
	char part1[] = "Hello, ";
	char part2[] = "World!";

	struct iovec iov[2];
	iov[0].iov_base = part1;
	iov[0].iov_len = strlen(part1);
	iov[1].iov_base = part2;
	iov[1].iov_len = strlen(part2);

	struct msghdr msg = { 0 };
	msg.msg_iov = iov;
	msg.msg_iovlen = 2;

	ssize_t sent = sendmsg(server_fd, &msg, MSG_NOSIGNAL);
	assert(sent == (ssize_t)(strlen(part1) + strlen(part2)));

	/* Receive on client */
	char buf[64] = { 0 };
	ssize_t received = recv(client, buf, sizeof(buf), 0);
	assert(received == sent);
	assert(strcmp(buf, "Hello, World!") == 0);

	/* Clean up */
	close(server_fd);
	close(client);
	close(listener);

	printf("OK (port=%d, sent=%d)\n", port, (int)sent);
	return 0;
}

static int test_nonblocking(void)
{
	printf("  test_nonblocking: ");

	int fds[2];

	/* Create a connected pair */
	int listener = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = 0;
	bind(listener, (struct sockaddr*)&addr, sizeof(addr));
	int addrlen = sizeof(addr);
	getsockname(listener, (struct sockaddr*)&addr, &addrlen);
	listen(listener, 1);

	fds[0] = socket(AF_INET, SOCK_STREAM, 0);
	connect(fds[0], (struct sockaddr*)&addr, sizeof(addr));
	fds[1] = accept(listener, NULL, NULL);
	close(listener);

	/* Set non-blocking */
	fcntl(fds[0], F_SETFL, O_NONBLOCK);

	/* Read should return error (nothing to read) */
	char buf[16];
	ssize_t rc = read(fds[0], buf, sizeof(buf));
	/* On POSIX: -1 with EAGAIN. On Windows: -1 with WSAEWOULDBLOCK */
	assert(rc < 0 || rc == 0);

	/* Write some data and then read should succeed */
	send(fds[1], "test", 4, 0);

#ifdef _WIN32
	Sleep(10);
#else
	usleep(10000);
#endif

	rc = read(fds[0], buf, sizeof(buf));
	assert(rc == 4);
	assert(memcmp(buf, "test", 4) == 0);

	close(fds[0]);
	close(fds[1]);

	printf("OK\n");
	return 0;
}

int main(void)
{
	printf("neatvnc TCP compatibility tests:\n");

	test_listen_accept_send();
	test_nonblocking();

	printf("All TCP tests passed.\n");
	return 0;
}
