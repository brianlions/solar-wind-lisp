/*
 * file name:           src/swl_utils.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 01:49:21 2014 CST
 */

#ifndef _SOLAR_WIND_LIST_UTILS_H_
#define _SOLAR_WIND_LIST_UTILS_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include <sstream>
#include <string>

#include "gnu_attributes.h"

#ifndef array_size
#define array_size(a) (sizeof(a) / sizeof(a[0]))
#endif

namespace SolarWindLisp
{
namespace Utils
{

/*
 * Use class (instead of namespace) and static methods to enforce every
 * reference to any of these methods are prefixed by the class name.
 */
class Misc
{
public:
    static std::string repeat(const char * pat, int times)
    {
        std::stringstream ss;
        for (int i = 0; i < times; ++i) {
            ss << pat;
        }
        return ss.str();
    }
};

class Time
{
public:
    static int64_t timestamp_msec(const struct timeval * ptv = NULL) ALWAYS_INLINE
    {
        struct timeval now;
        if (!ptv) {
            gettimeofday(&now, NULL);
            ptv = &now;
        }
        return static_cast<int64_t>(ptv->tv_sec * 1000 + ptv->tv_usec / 1000);
    }

    static int64_t timestamp_usec(const struct timeval * ptv = NULL) ALWAYS_INLINE
    {
        struct timeval now;
        if (!ptv) {
            gettimeofday(&now, NULL);
            ptv = &now;
        }
        return static_cast<int64_t>(ptv->tv_sec * 1000000 + ptv->tv_usec);
    }

    static double timestamp(const struct timeval * ptv = NULL) ALWAYS_INLINE
    {
        struct timeval now;
        if (!ptv) {
            gettimeofday(&now, NULL);
            ptv = &now;
        }
        return (1.0 * ptv->tv_sec + ptv->tv_usec / 1000000.0);
    }

    static int sleep_msec(unsigned int interval_msec,
            unsigned int * remain_msec = NULL)
    {
        struct timespec intv, rem;
        intv.tv_sec = (time_t) interval_msec / 1000;
        intv.tv_nsec = (long int) (interval_msec % 1000) * 1000000;

        if (nanosleep(&intv, remain_msec ? &rem : NULL) < 0) {
            if (errno == EINTR && remain_msec) {
                *remain_msec = static_cast<unsigned int>(
                        rem.tv_sec * 1000 + rem.tv_nsec / 1000000);
            }
            return -1;
        }

        if (remain_msec) {
            *remain_msec = 0;
        }
        return 0;
    }

    static int sleep(double interval_sec, double * remain_sec = NULL)
    {
        unsigned int n = static_cast<unsigned int>(interval_sec * 1000);
        unsigned int r = 0;
        int ret = sleep_msec(n, remain_sec ? &r : NULL);
        if (remain_sec) {
            if (ret < 0) {
                if (r) {
                    *remain_sec = 0.001 * r;
                }
            }
            else {
                *remain_sec = 0;
            }
        }
        return ret;
    }

    static char * hires_ascii_time(char * buf, size_t buflen,
            const struct timeval * ptv = NULL, bool utc = false)
    {
        struct timeval now;
        if (!ptv) {
            gettimeofday(&now, NULL);
            ptv = &now;
        }

        struct tm broken;
        utc ? gmtime_r(&(ptv->tv_sec), &broken)
            : localtime_r(&(ptv->tv_sec), &broken);

        char format[64];
        sprintf(format, "%%Y.%%m.%%d-%%H:%%M:%%S.%06d-%%Z",
                static_cast<int>(ptv->tv_usec));
        strftime(buf, buflen, format, &broken);
        return buf;
    }
};

} // namespace Utils
} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LIST_UTILS_H_
