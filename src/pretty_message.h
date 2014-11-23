#ifndef _SOLAR_WIND_LIST_PRETTY_MESSAGE_H_
#define _SOLAR_WIND_LIST_PRETTY_MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef USE_PRETTY_MESSAGE
#define USE_PRETTY_MESSAGE
#endif

#if defined(USE_PRETTY_MESSAGE)

#define PRETTY_MESSAGE(stream, fmt, ...)                                        \
do {                                                                            \
    time_t now;                                                                 \
    struct tm now_broken;                                                       \
    char time_str[64];                                                          \
    time(&now);                                                                 \
    localtime_r(&now, &now_broken);                                             \
    strftime(time_str, sizeof(time_str), "%Y.%m.%d-%H:%M:%S", &now_broken);     \
    (void) fprintf((stream), "[%s %s:%04d:%s] " fmt "\n", time_str,             \
        __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__);                      \
} while (0)

#define DEV_MESSAGE(fmt, ...)                                                   \
do {                                                                            \
    time_t now;                                                                 \
    struct tm now_broken;                                                       \
    char time_str[64];                                                          \
    time(&now);                                                                 \
    localtime_r(&now, &now_broken);                                             \
    strftime(time_str, sizeof(time_str), "%Y.%m.%d-%H:%M:%S", &now_broken);     \
    (void) fprintf(stderr, "[%s %s:%04d:%s] " fmt "\n", time_str,               \
        __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__);                      \
} while (0)

#if 0
#define BEGIN_FUNC() do { fprintf(stderr, "+++++ %s:%d:%s +++\n", __FILE__, __LINE__, __FUNCTION__); } while (0)
#define END_FUNC()   do { fprintf(stderr, "----- %s:%d:%s ---\n", __FILE__, __LINE__, __FUNCTION__); } while (0)
#else
#define BEGIN_FUNC() PRETTY_MESSAGE(stderr, "entering ...")
#define END_FUNC()   PRETTY_MESSAGE(stderr, "leaving ...")
#endif

#else //---------------------------------------------------------------------------------------------------------------
#define PRETTY_MESSAGE(stream, fmt, ...)
#define DEV_MESSAGE(fmt, ...)
#define BEGIN_FUNC()
#define END_FUNC()
#endif

#endif // _SOLAR_WIND_LIST_PRETTY_MESSAGE_H_