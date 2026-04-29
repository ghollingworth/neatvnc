/*
 * Test platform compatibility shims for neatvnc.
 *
 * Tests:
 * 1. pthread_mutex (critical section on Windows)
 * 2. pthread_cond (condition variable on Windows)
 * 3. pthread_create/join
 * 4. getrandom / BCryptGenRandom
 * 5. DRM fourcc constants
 *
 * Should pass on both POSIX and Windows.
 */

#ifdef _WIN32
#include "compat/platform.h"
#else
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/random.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

/* DRM fourcc - use compat on Windows, system on Linux */
#ifdef _WIN32
/* Already included via platform.h -> drm_fourcc.h */
#else
#include <libdrm/drm_fourcc.h>
#endif

static int test_mutex(void)
{
	printf("  test_mutex: ");

	pthread_mutex_t m;
	assert(pthread_mutex_init(&m, NULL) == 0);
	assert(pthread_mutex_lock(&m) == 0);
	assert(pthread_mutex_unlock(&m) == 0);
	assert(pthread_mutex_destroy(&m) == 0);

	printf("OK\n");
	return 0;
}

static int shared_counter = 0;
static pthread_mutex_t counter_mutex;

static void* thread_increment(void* arg)
{
	int n = *(int*)arg;
	for (int i = 0; i < n; i++) {
		pthread_mutex_lock(&counter_mutex);
		shared_counter++;
		pthread_mutex_unlock(&counter_mutex);
	}
	return NULL;
}

static int test_threads(void)
{
	printf("  test_threads: ");

	shared_counter = 0;
	pthread_mutex_init(&counter_mutex, NULL);

	int count = 1000;
	pthread_t t1, t2;
	assert(pthread_create(&t1, NULL, thread_increment, &count) == 0);
	assert(pthread_create(&t2, NULL, thread_increment, &count) == 0);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_mutex_destroy(&counter_mutex);

	assert(shared_counter == 2000);

	printf("OK (counter=%d)\n", shared_counter);
	return 0;
}

static pthread_mutex_t cond_mutex;
static pthread_cond_t cond_var;
static int cond_ready = 0;

static void* cond_waiter(void* arg)
{
	(void)arg;
	pthread_mutex_lock(&cond_mutex);
	while (!cond_ready)
		pthread_cond_wait(&cond_var, &cond_mutex);
	pthread_mutex_unlock(&cond_mutex);
	return NULL;
}

static int test_condvar(void)
{
	printf("  test_condvar: ");

	cond_ready = 0;
	pthread_mutex_init(&cond_mutex, NULL);
	pthread_cond_init(&cond_var, NULL);

	pthread_t waiter;
	assert(pthread_create(&waiter, NULL, cond_waiter, NULL) == 0);

	/* Give waiter time to block */
#ifdef _WIN32
	Sleep(50);
#else
	usleep(50000);
#endif

	/* Signal it */
	pthread_mutex_lock(&cond_mutex);
	cond_ready = 1;
	pthread_cond_signal(&cond_var);
	pthread_mutex_unlock(&cond_mutex);

	pthread_join(waiter, NULL);
	pthread_mutex_destroy(&cond_mutex);
	pthread_cond_destroy(&cond_var);

	printf("OK\n");
	return 0;
}

static int test_random(void)
{
	printf("  test_random: ");

	uint8_t buf[32] = { 0 };
	ssize_t rc = getrandom(buf, sizeof(buf), 0);
	assert(rc == sizeof(buf));

	/* Check it's not all zeros (astronomically unlikely if random works) */
	int nonzero = 0;
	for (size_t i = 0; i < sizeof(buf); i++)
		if (buf[i] != 0)
			nonzero++;
	assert(nonzero > 0);

	printf("OK (%d non-zero bytes)\n", nonzero);
	return 0;
}

static int test_drm_fourcc(void)
{
	printf("  test_drm_fourcc: ");

	/* Verify key format constants have expected values */
	assert(DRM_FORMAT_XRGB8888 != 0);
	assert(DRM_FORMAT_ARGB8888 != 0);
	assert(DRM_FORMAT_XRGB8888 != DRM_FORMAT_ARGB8888);
	assert(DRM_FORMAT_BGRX8888 != DRM_FORMAT_RGBX8888);

	printf("OK (XRGB8888=0x%08x, ARGB8888=0x%08x)\n",
	       DRM_FORMAT_XRGB8888, DRM_FORMAT_ARGB8888);
	return 0;
}

int main(void)
{
#ifdef _WIN32
	printf("neatvnc compat tests (Windows):\n");
#else
	printf("neatvnc compat tests (POSIX):\n");
#endif

	test_mutex();
	test_threads();
	test_condvar();
	test_random();
	test_drm_fourcc();

	printf("All compat tests passed.\n");
	return 0;
}
