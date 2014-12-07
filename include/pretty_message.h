/*
 * file name:           include/pretty_message.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_PRETTY_MESSAGE_H_
#define _SOLAR_WIND_LISP_PRETTY_MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

//#ifndef USE_PRETTY_MESSAGE
//#define USE_PRETTY_MESSAGE
//#endif

#if defined(USE_PRETTY_MESSAGE)

#define PRETTY_MESSAGE(stream, fmt, ...)                                        \
do {                                                                            \
    char time_str[64];                                                          \
    SolarWindLisp::Utils::Time::hires_ascii_time(time_str, sizeof(time_str));   \
    (void) fprintf((stream), "[%s %s:%04d:%s] " fmt "\n", time_str,             \
        __FILE__, __LINE__, __FUNCTION__, ## __VA_ARGS__);                      \
} while (0)

#define DEV_MESSAGE(fmt, ...)                                                   \
do {                                                                            \
    char time_str[64];                                                          \
    SolarWindLisp::Utils::Time::hires_ascii_time(time_str, sizeof(time_str));   \
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

#endif // _SOLAR_WIND_LISP_PRETTY_MESSAGE_H_
