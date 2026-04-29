/*
 * Minimal drm_fourcc.h definitions for neatvnc on Windows.
 *
 * These are just the fourcc code definitions — no kernel dependency needed.
 * Extracted from Linux kernel's include/uapi/drm/drm_fourcc.h.
 */

#pragma once

#ifndef DRM_FORMAT_BIG_ENDIAN
#define DRM_FORMAT_BIG_ENDIAN (1U << 31)
#endif

#define fourcc_code(a, b, c, d) \
	((uint32_t)(a) | ((uint32_t)(b) << 8) | \
	 ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define DRM_FORMAT_INVALID	0

#define DRM_FORMAT_XRGB4444	fourcc_code('X', 'R', '1', '2')
#define DRM_FORMAT_XBGR4444	fourcc_code('X', 'B', '1', '2')
#define DRM_FORMAT_RGBX4444	fourcc_code('R', 'X', '1', '2')
#define DRM_FORMAT_BGRX4444	fourcc_code('B', 'X', '1', '2')
#define DRM_FORMAT_ARGB4444	fourcc_code('A', 'R', '1', '2')
#define DRM_FORMAT_ABGR4444	fourcc_code('A', 'B', '1', '2')
#define DRM_FORMAT_RGBA4444	fourcc_code('R', 'A', '1', '2')
#define DRM_FORMAT_BGRA4444	fourcc_code('B', 'A', '1', '2')

#define DRM_FORMAT_XRGB8888	fourcc_code('X', 'R', '2', '4')
#define DRM_FORMAT_XBGR8888	fourcc_code('X', 'B', '2', '4')
#define DRM_FORMAT_RGBX8888	fourcc_code('R', 'X', '2', '4')
#define DRM_FORMAT_BGRX8888	fourcc_code('B', 'X', '2', '4')
#define DRM_FORMAT_ARGB8888	fourcc_code('A', 'R', '2', '4')
#define DRM_FORMAT_ABGR8888	fourcc_code('A', 'B', '2', '4')
#define DRM_FORMAT_RGBA8888	fourcc_code('R', 'A', '2', '4')
#define DRM_FORMAT_BGRA8888	fourcc_code('B', 'A', '2', '4')

#define DRM_FORMAT_XRGB2101010	fourcc_code('X', 'R', '3', '0')
#define DRM_FORMAT_XBGR2101010	fourcc_code('X', 'B', '3', '0')
#define DRM_FORMAT_RGBX1010102	fourcc_code('R', 'X', '3', '0')
#define DRM_FORMAT_BGRX1010102	fourcc_code('B', 'X', '3', '0')
#define DRM_FORMAT_ARGB2101010	fourcc_code('A', 'R', '3', '0')
#define DRM_FORMAT_ABGR2101010	fourcc_code('A', 'B', '3', '0')
#define DRM_FORMAT_RGBA1010102	fourcc_code('R', 'A', '3', '0')
#define DRM_FORMAT_BGRA1010102	fourcc_code('B', 'A', '3', '0')

#define DRM_FORMAT_RGB565	fourcc_code('R', 'G', '1', '6')
#define DRM_FORMAT_BGR565	fourcc_code('B', 'G', '1', '6')

#define DRM_FORMAT_RGB888	fourcc_code('R', 'G', '2', '4')
#define DRM_FORMAT_BGR888	fourcc_code('B', 'G', '2', '4')

#define DRM_FORMAT_XRGB1555	fourcc_code('X', 'R', '1', '5')
#define DRM_FORMAT_XBGR1555	fourcc_code('X', 'B', '1', '5')
#define DRM_FORMAT_ARGB1555	fourcc_code('A', 'R', '1', '5')
#define DRM_FORMAT_ABGR1555	fourcc_code('A', 'B', '1', '5')

/* Format modifiers */
#define DRM_FORMAT_MOD_NONE	0
#define DRM_FORMAT_MOD_LINEAR	0

#define fourcc_mod_code(vendor, val) \
	((((uint64_t)(vendor)) << 56) | ((val) & 0x00ffffffffffffffULL))

#define DRM_FORMAT_MOD_VENDOR_NONE    0
#define DRM_FORMAT_MOD_VENDOR_INTEL   0x01
#define DRM_FORMAT_MOD_VENDOR_AMD     0x02
#define DRM_FORMAT_MOD_VENDOR_NVIDIA  0x03
#define DRM_FORMAT_MOD_VENDOR_SAMSUNG 0x04
#define DRM_FORMAT_MOD_VENDOR_ARM     0x08

/* Intel modifiers (used by pixels.c for tiling) */
#define I915_FORMAT_MOD_X_TILED  fourcc_mod_code(DRM_FORMAT_MOD_VENDOR_INTEL, 1)
#define I915_FORMAT_MOD_Y_TILED  fourcc_mod_code(DRM_FORMAT_MOD_VENDOR_INTEL, 2)
#define I915_FORMAT_MOD_Yf_TILED fourcc_mod_code(DRM_FORMAT_MOD_VENDOR_INTEL, 3)

/* DCC / retile stubs for AMD — not used on Windows but referenced in pixels.c */
#ifndef DCC
#define DCC 0
#endif
#ifndef DCC_RETILE
#define DCC_RETILE 0
#endif
#ifndef DCB
#define DCB 0
#endif

/* AMD modifier extraction — stub for Windows */
#ifndef AMD_FMT_MOD_GET
#define AMD_FMT_MOD_GET(field, value) 0
#endif
