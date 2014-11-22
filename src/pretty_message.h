#ifndef _SOLAR_WIND_LIST_PRETTY_MESSAGE_H_
#define _SOLAR_WIND_LIST_PRETTY_MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>

#ifndef USE_PRETTY_MESSAGE
#define USE_PRETTY_MESSAGE
#endif

#if defined(USE_PRETTY_MESSAGE)

#define PRETTY_MESSAGE(stream, fmt, ...)                                  \
do {                                                                      \
    /*char time_str[64];*/                                                    \
    (void) fprintf((stream), "[%s %s:%04d:%s] " fmt "\n", "unknown-time", \
        __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__);                \
} while (0)

#define DEV_MESSAGE(fmt, ...)                                             \
do {                                                                      \
    /*char time_str[64];*/                                                    \
    (void) fprintf(stderr, "[%s %s:%04d:%s] " fmt "\n", "unknown-time",   \
        __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__);                \
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
