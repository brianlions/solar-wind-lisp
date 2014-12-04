/*
 * file name:           src/gnu_attributes.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _GNU_ATTRIBUTES_H_
#define _GNU_ATTRIBUTES_H_

#ifdef __GNUC__
#define likely(x)   __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#ifdef __GNUC__
#define PRINTF_FORMAT(M, N)       __attribute__((format(printf, (M), (N))))
#define SCANF_FORMAT(M, N)        __attribute__((format(scanf, (M), (N))))
#define STRFTIME_FORMAT(M)        __attribute__((format(strftime, (M), 0)))
#define CLASS_PRINTF_FORMAT(M, N) __attribute__((format(printf, (M + 1), (N + 1))))
#define CLASS_SCANF_FORMAT(M, N)  __attribute__((format(scanf, (M + 1), (N + 1))))
#define CLASS_STRFTIME_FORMAT(M)  __attribute__((format(strftime, (M + 1), 0)))
#define INLINE                    __inline__
#define ALWAYS_INLINE             __attribute__((always_inline))
#define WARN_UNUSED_RESULT        __attribute__((warn_unused_result))
#define UNUSED                    __attribute__((unused))
#define DEPRECATED                __attribute__((deprecated))
#else
#define PRINTF_FORMAT(M, N)
#define SCANF_FORMAT(M, N)
#define STRFTIME_FORMAT(M)
#define CLASS_PRINTF_FORMAT(M, N)
#define CLASS_SCANF_FORMAT(M, N)
#define CLASS_STRFTIME_FORMAT(M)
#define INLINE
#define ALWAYS_INLINE
#define WARN_UNUSED_RESULT
#define UNUSED
#define DEPRECATED
#endif

#endif // _GNU_ATTRIBUTES_H_
