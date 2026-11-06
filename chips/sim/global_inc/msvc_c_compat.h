/* msvc_c_compat.h ? small MSVC C11 shims for NuttX sim host */
#ifndef AUTOC_MSVC_C_COMPAT_H
#define AUTOC_MSVC_C_COMPAT_H
#ifdef _MSC_VER
#ifndef __attribute__
#define __attribute__(x)
#endif
#ifndef __builtin_expect
#define __builtin_expect(x, y) (x)
#endif
#ifndef __unused
#define __unused
#endif
#endif
#endif
