#pragma once
#ifdef _WIN32
/* USDT probes not available on Windows */
#ifndef DTRACE_PROBE
#define DTRACE_PROBE(p, n)
#define DTRACE_PROBE1(p, n, a)
#define DTRACE_PROBE2(p, n, a, b)
#define DTRACE_PROBE3(p, n, a, b, c)
#define DTRACE_PROBE4(p, n, a, b, c, d)
#endif
#else
#include_next <sys/sdt.h>
#endif
