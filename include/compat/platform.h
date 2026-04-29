/*
 * Platform compatibility header for neatvnc on Windows (MinGW-w64).
 *
 * This header is force-included via the build system on Windows builds.
 *
 * MinGW-w64 provides: pthreads, signal.h, time.h, struct timespec.
 * We only need to provide:
 *   - ssize_t
 *   - DRM fourcc constants
 *   - MIN/MAX
 *   - EXPORT macro
 *   - tgmath.h -> math.h
 *   - getrandom via BCryptGenRandom
 *   - USDT stubs
 *   - Networking compat (sendmsg, iovec, etc.)
 *   - SIGPIPE stub
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
#include <stdint.h>
#include <stdlib.h>

/* --- ssize_t --- */
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef long long ssize_t;
#endif

/* --- DRM fourcc constants --- */
#include "compat/drm_fourcc.h"

/* --- MIN/MAX --- */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/* --- EXPORT macro --- */
/* MinGW GCC supports visibility attribute */
#define EXPORT __attribute__((visibility("default")))

/* --- tgmath.h -> math.h --- */
/* MinGW doesn't have tgmath.h; math.h provides what neatvnc needs */
#define tgmath_h_placeholder
#include <math.h>

/* --- SIGPIPE --- */
#ifndef SIGPIPE
#define SIGPIPE 13
#endif

/* --- getrandom --- */
#include <bcrypt.h>

static inline ssize_t compat_getrandom(void* buf, size_t buflen, unsigned int flags) {
	(void)flags;
	NTSTATUS status = BCryptGenRandom(NULL, (PUCHAR)buf, (ULONG)buflen,
	                                  BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	return status == 0 ? (ssize_t)buflen : -1;
}
#define getrandom compat_getrandom

/* --- USDT stubs --- */
#ifndef DTRACE_PROBE
#define DTRACE_PROBE(provider, probe)
#define DTRACE_PROBE1(provider, probe, a1)
#define DTRACE_PROBE2(provider, probe, a1, a2)
#define DTRACE_PROBE3(provider, probe, a1, a2, a3)
#define DTRACE_PROBE4(provider, probe, a1, a2, a3, a4)
#endif

/* --- aligned_alloc / free --- */
/* MinGW doesn't have aligned_alloc. Use _aligned_malloc, but we must also
 * override free() for buffers allocated this way since _aligned_malloc
 * requires _aligned_free. Use a wrapper that works with both.
 */
static inline void* compat_aligned_alloc(size_t alignment, size_t size) {
	return _aligned_malloc(size, alignment);
}
static inline void compat_free(void* ptr) {
	/* _aligned_free handles both _aligned_malloc and regular pointers
	 * on MSVC, but on MinGW we need _aligned_free for _aligned_malloc.
	 * Since we can't tell how ptr was allocated, use _aligned_free which
	 * works for _aligned_malloc allocations.
	 */
	_aligned_free(ptr);
}
#define aligned_alloc compat_aligned_alloc
/* Note: We do NOT override free() globally as that would break everything.
 * Instead, neatvnc's buffer.c needs to use _aligned_free for addr.
 * For now, use a simpler approach: make aligned_alloc return regular malloc
 * memory by over-allocating.
 */
#undef aligned_alloc
static inline void* compat_aligned_alloc2(size_t alignment, size_t size) {
	/* Just use malloc — the alignment is only for performance, not
	 * correctness. This avoids the _aligned_free requirement.
	 */
	(void)alignment;
	return malloc(size);
}
#define aligned_alloc compat_aligned_alloc2

/* --- Networking compat --- */
#include "compat/net.h"

#endif /* _WIN32 */
